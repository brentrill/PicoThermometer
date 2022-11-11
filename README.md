# Pico Thermometer
Pico Thermometer makes use of the Raspberry Pi Pico's internal temperature sensor to move a servo akin to a dial thermometer, including a potentiometer to calibrate the needle if needed. The application features a micro-ROS publisher for seamless integration into a larger robotic ecosystem with ROS 2. 

<p align="center">
  <img src="https://images2.imgbox.com/96/20/LDNveEpF_o.jpg" alt="Pico Pic" width="738">
</p>

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
Following the Pico [pin-out diagram](https://datasheets.raspberrypi.com/pico/Pico-R3-A4-Pinout.pdf), we power the servo by connecting it to PIN 40, and PIN 38 for ground. It's input is connected to PIN 1 (GP0).

The potentiometer, used to calibrate the servo, receives voltage from PIN 36, outputs to PIN 31 (ADC0), and is grounded by PIN 23.

This is all the necessary wiring as the Pico can read temperature from it's RP2040 chip internally.

### Software Setup
I include my CMake build folder so that one can easily flash the Pico with the program without all the software setup.
I'll also explain my environment if you wish to build it as well.

Install ROS2 Foxy with Ubuntu Linux 20.04.5.
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

![Published Results](https://images2.imgbox.com/28/ea/ndTyMbzM_o.png)
