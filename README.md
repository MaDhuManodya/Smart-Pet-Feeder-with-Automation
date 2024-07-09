# Smart Pet Feeder with Automation

## Overview

The Smart Pet Feeder is an innovative solution designed to automate and enhance the feeding process for pets. It incorporates advanced features such as motion detection, portion control, smartphone integration, and a feedback mechanism to ensure pets receive proper nutrition. This project leverages the ESP32-WROOM-DA module, ESP32-CAM module, PIR motion sensor, and a Telegram bot for remote control and monitoring.

## Features

- **Food Intake Tracking**: Monitors and measures the pet's daily food consumption using sensors.
- **Portion Control**: Dispenses the right amount of food based on the pet's dietary needs and feeding schedule.
- **Smartphone Integration**: Allows pet owners to remotely monitor and adjust feeding schedules, access feeding history, and receive notifications via a smartphone app.
- **Feeding Schedule Customization**: Enables customization of feeding schedules according to the pet's needs.
- **Separate Command Execution**: Provides individual commands for each function to ensure reliable operation even if the system is partially out of control.

## Hardware Components

- **ESP32-WROOM-DA Module**:
  - Onboard Wi-Fi and Bluetooth.
  - Digital-to-Analog conversion capabilities.
  - Suitable for IoT and embedded projects with low power consumption and a wide range of peripherals.

- **ESP32-CAM Module**:
  - Integrates a camera for capturing images and videos.
  - Supports Wi-Fi and Bluetooth connectivity.
  - Ideal for applications like surveillance, image recognition, and remote monitoring.

- **PIR Motion Sensor**:
  - Detects motion by sensing changes in infrared radiation.
  - Features adjustable sensitivity settings and wide-angle coverage.
  - Low energy consumption, suitable for battery-operated applications.

- **12V Solenoid Water Pump**:
  - Used for transferring water in various applications.
  - Driven by 12V DC, compatible with batteries and solar panels.
  - Compact, lightweight, and designed for efficiency and reliability.

- **Servo Motor with Metal Wheel**:
  - Converts electrical energy into rotational motion.
  - Controlled by pulse width modulation signals.
  - Suitable for rotating objects and mechanisms with moderate load-bearing capacity.

- **Load Cell Sensor**:
  - Measures force or mass based on material deformation and electric charge.
  - Provides precise and reliable results, widely used in industrial and commercial applications.

## Working Procedure

1. **Setup Time Ranges**:
   - Define feeding times: Morning (8:00 am - 9:00 am), Afternoon (2:00 pm - 3:00 pm), Night (7:00 pm - 8:00 pm).

2. **Motion Detection and Alert System**:
   - Detects motion near the feeder using the PIR sensor.
   - Sends alerts and images to the Telegram bot, and displays messages on the LCD.

3. **Manual Photo Capture**:
   - Allows manual triggering to capture images outside automated processes.

4. **Detection of Pet Proximity**:
   - Monitors the pet's approach to the feeder.

5. **Feeding Process Initiation**:
   - Receives `/feed` command via Telegram bot.
   - Dispenses food using the servo motor and sends confirmation messages.

6. **Weight Measurement**:
   - Measures and displays the weight of the food tower.
   - Sends weight information to the Telegram bot.

7. **Water Pumping Process**:
   - Initiates water pumping after feeding.
   - Sends status messages to the Telegram bot and displays updates on the LCD.

8. **Final Weight Reporting**:
   - Sends a report with the final weight and quantity of food distributed to the Telegram bot.
   - Displays these values on the LCD.

## Installation and Setup

1. **Hardware Assembly**:
   - Connect the ESP32-WROOM-DA and ESP32-CAM modules to the respective sensors and actuators.
   - Ensure proper wiring for the PIR sensor, solenoid water pump, servo motor, and load cell sensor.

2. **Software Configuration**:
   - Install the necessary libraries and SDKs for ESP32 development.
   - Configure the Telegram bot and obtain the bot token.
   - Upload the firmware to the ESP32 modules using the Arduino IDE or PlatformIO.

3. **Running the System**:
   - Power on the system and ensure all components are functioning.
   - Use the Telegram bot to interact with the feeder, monitor status, and receive alerts.

## Contributing

Contributions are welcome! Please fork the repository, make your changes, and submit a pull request. Ensure your code follows the project's coding standards and includes appropriate documentation.
