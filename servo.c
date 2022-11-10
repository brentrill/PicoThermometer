#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Helper functions to control servo by degrees
// Takes in pin # and position value,
// sets servo desired position
void initDegree(int pin, float degree)
{
    pwm_set_gpio_level(pin, (degree / 20000.f) * 39062.f);
}

// Calls necessary functions for servo initialization
void initServo(int pin, float startDegree)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_config config = pwm_get_default_config();

    // divides sys clock (125MHz) by 64
    pwm_config_set_clkdiv(&config, 64.f);
    pwm_config_set_wrap(&config, 39062.f);

    pwm_init(slice_num, &config, true);

    initDegree(pin, startDegree);
}
