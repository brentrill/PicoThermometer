#include <stdio.h>
#include <unistd.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/string.h>
#include <rmw_microros/rmw_microros.h>

#include "pico/stdlib.h"
#include "pico_uart_transports.h"
#include "servo.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

// necessary for micro-ROS publisher
#define ARRAY_LEN 200
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

// Globals
// Potentiometer calibration scale
int scale = 10;

// set temperature range in F
int minTemp = 50;
int maxTemp = 100;

// covers 180 deg for my SG90 servo
int minDegree = 350;
int maxDegree = 2270;  // 2600 is abs max but > 180deg
int currentPos = (2270 - 350) / 2; // mid point, maxDegree - minDegree / 2

// output pin on pico wired to servo
int pin = 0;

const uint LED_PIN = 25;

rcl_publisher_t publisher;
std_msgs__msg__String msg;
int counter = 0;

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
	(void) last_call_time;
	if (timer != NULL) {
		msg.data.size = strlen(msg.data.data);
		RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
		printf("I have published: \"%s\"\n", msg.data.data);
	}
}

int main()
{
    // Setting up the publisher
    rmw_uros_set_custom_transport(
		true,
		NULL,
		pico_serial_transport_open,
		pico_serial_transport_close,
		pico_serial_transport_write,
		pico_serial_transport_read
	);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    rcl_timer_t timer;
    rcl_node_t node;
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;
    rclc_executor_t executor = rclc_executor_get_zero_initialized_executor();

    // Wait for agent successful ping for 2 minutes.
    const int timeout_ms = 1000; 
    const uint8_t attempts = 120;

    rcl_ret_t ret = rmw_uros_ping_agent(timeout_ms, attempts);

    if (ret != RCL_RET_OK)
    {
        // Unreachable agent, exiting program.
        return ret;
    }

    rclc_support_init(&support, 0, NULL, &allocator);

    rclc_node_init_default(&node, "pico_node", "", &support);
    rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
        "pico_publisher");

    rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(timeout_ms),
        timer_callback);

    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_timer(&executor, &timer);

    gpio_put(LED_PIN, 1);

    msg.data.data = (char * ) malloc(ARRAY_LEN * sizeof(char));
	msg.data.size = 0;
	msg.data.capacity = ARRAY_LEN;

    // Initializations for application function
    initServo(pin, currentPos);

    // configure ADC
    adc_init();
    adc_gpio_init(26); // Potentiometer
    adc_set_temp_sensor_enabled(true);
    while (true)
    {
        // Checks for new data at DDS-queue once
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));

        // Temperature Read
        adc_select_input(4);
        uint16_t raw = adc_read();// reads raw 12bit output from ADC
        // 3.3V max, bitwise shift of max 12bit integer
        const float conversion = 3.3f / (1<<12); 
        float voltage = raw * conversion;
        // temp formula given by Pico RP2040 documention
        float temperature = 27 - (voltage - 0.706) / 0.001721;
        temperature = temperature * (9.0/5) + 32; // C to F
        
        // Finds goal servo position
        // servo 0 to 180 deg = 350 to 2270
        float ratio = (temperature - minTemp) / (maxTemp - minTemp);
        int servoPos = (int)(
                    (1 - ratio) * (maxDegree - minDegree) + minDegree);

        //Potentiometer calibration
        adc_select_input(0);
        uint16_t potRead = adc_read();
        int calibration = ((int)(
                        (potRead * conversion) * 10.) - 16) * scale;
        servoPos = servoPos + calibration;

        // Moving Servo
        if (servoPos > currentPos) {
            for (currentPos++; currentPos < servoPos; currentPos++){
                initDegree(pin, currentPos);
                sleep_ms(20);
            }
        } else if (servoPos < currentPos){
            for (currentPos--; currentPos > servoPos; currentPos--){
                initDegree(pin, currentPos);
                sleep_ms(20);
            }
        }

        // Constructing published string
        sprintf(msg.data.data,
            "#%d\nTemp: %.1f F\nPotV: %.1f V",
            counter++,
            temperature,
            potRead * conversion);

        sleep_ms(1000);
    }
    return 0;
}
