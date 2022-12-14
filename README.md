# Pico Thermometer
Pico Thermometer makes use of the Raspberry Pi Pico's internal temperature sensor to move a servo akin to a dial thermometer, including a potentiometer to calibrate the needle position if needed. The application features a micro-ROS publisher for seamless integration into a larger robotic ecosystem with ROS 2. 

Update: Added a new ROS 2 subscriber node that displays data from the Pico in a GUI using PyQT5.

<p align="center">
  <img src="https://images2.imgbox.com/96/20/LDNveEpF_o.jpg" alt="Pico Pic" width="738">
</p>

## How It Works
1. We make use of 2 of the Pico's analog-to-digital converters. One reads the voltage of a diode that is a proportion of temperature, and the other is used by the calibrating potentiometer.
2. Servos work by moving to a specific position value given by its input. By setting the desired temperature range and translating 180 degrees of servo rotation into position values, we are able to tell the servo to move to a specific position based on temperature.
3. After initializing all inputs and outputs according to documentation, the main while loop takes place in `microtemp.c`. Each iteration reads temperature and converts it to Fahrenheit, translates this into servo position by using a ratio and the calibration scale, then moves the servo by incrementing its position if needed.

## Installation
For a general in-depth setup guide to working with micro-ROS on the Raspberry Pi Pico, refer to [this guide](https://ubuntu.com/blog/getting-started-with-micro-ros-on-raspberry-pi-pico). I will only explain what make my project unique in this readme.

### Hardware Setup
#### Required Hardware:
- Raspberry Pi Pico w/ Headers
- Breadboard
- Servo that can operate at 3.3-5 V. I use an SG90.
- 10k Ohm Potentiometer
- Jump wires

#### Wiring
Following the Pico [pin-out diagram](https://datasheets.raspberrypi.com/pico/Pico-R3-A4-Pinout.pdf), we power the servo by connecting it to PIN 40, and then PIN 38 for ground. Its input is connected to PIN 1 (GP0).

The potentiometer, used to calibrate the servo, receives voltage from PIN 36, outputs to PIN 31 (ADC0), and is grounded by PIN 23.

### Software Setup
#### Prerequisites
- Ubuntu Linux 20.04.5
- ROS2 Foxy

Follow [this previously mentioned guide](https://ubuntu.com/blog/getting-started-with-micro-ros-on-raspberry-pi-pico) in order to make sure the environment is working.
After cloning this repository, make sure the settings.json file in the .vscode folder points to your PICO_SDK_PATH.
Then, build the project in VS Code as in the guide.

## Running the Program
Connect the Pico by USB while holding the BOOTSEL button. This will make it appear as an external media drive.

Navigate to the project directory in a new terminal. Flash the Pico by executing the command,

`cp build/temp.uf2 /media/$USER/RPI-RP2`

Like the linked guide, the main loop will not execute unless the micro-ROS agent is running. Start it with the command in a new terminal,

`micro-ros-agent serial --dev /dev/ttyACM0 baudrate=115200`

The LED will turn on if the code is executing. You can now see published data with the commands

`source /opt/ros/dashing/setup.bash`

`ros2 topic echo /pico_publisher`

<p align="center">
  <img src="https://images2.imgbox.com/28/ea/ndTyMbzM_o.png" alt="Published Results">
</p>

## Using the Python GUI
The GUI depends on `PyQt5==5.15.4`. It also uses `rclpy` and `std_msgs` as part of the ROS 2 distribution for the Python subscriber node. In the cloned repository directory, you can check for missing dependencies with the command

`rosdep install -i --from-path src --rosdistro foxy -y`

Once everything is installed, navigate to the first `py_pubsub` directory in a terminal and run the command 

`colcon build --symlink-install`

to build the package. Then, source the setup files with the command

`source install/setup.bash`

and run the program using

`ros2 run py_pubsub pico_subscriber`

Assuming the Pico is still connected and executing its program, we should see the message data displayed in a new, small window.

<p align="center">
  <img src="https://images2.imgbox.com/de/c0/QNTTySTP_o.png" alt="Python GUI">
</p>
