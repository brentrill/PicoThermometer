# Pico Thermometer
A micro-ROS publishing application for the Raspberry Pi Pico that senses temperature.
![Project Pic](https://images2.imgbox.com/88/3d/t7D450AO_o.jpg)

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
Connect the Pico by USB in Boot 
