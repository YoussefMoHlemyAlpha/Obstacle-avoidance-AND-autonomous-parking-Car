# Obstacle-avoidance-AND-autonomous-parking-Car
![System](https://github.com/user-attachments/assets/82d6de55-927c-4086-852a-99e36a5345f8)
🚗 Intelligent Vehicle Control System using ESP32
This project implements an intelligent vehicle control system based on the ESP32 microcontroller, designed to manage autonomous movement and safety using ultrasonic sensors, DC motors, and MQTT-based communication. The system supports multiple operating modes including Obstacle Detection, Parking Mode, and Stopping Mode.

🧠 Project Overview
The system is divided into two primary modules:

Hardware:

ESP32 Microcontroller

3 Ultrasonic Sensors for obstacle detection

L298N H-Bridge Motor Driver

4 DC Motors

Power Supply Unit

Software:

MQTT Protocol for communication

Control logic for switching between Obstacle Detection Mode, Parking Mode, and Stopping Mode

Sensor data acquisition and motor control

🔁 System Workflow
Ultrasonic Sensors continuously monitor the surroundings and send distance data to the ESP32 via ADC.

The ESP32 processes sensor data and decides motor control actions based on the current mode.

Commands are sent to the L298N motor driver via DAC, controlling the DC motors accordingly.

MQTT Protocol is used to receive mode-switching commands from a mobile or desktop application.

📡 Modes of Operation
Obstacle Detection Mode: Automatically halts or reroutes the vehicle when objects are detected.

Parking Mode: Enables semi-autonomous or remote-controlled parking.

Stopping Mode: Immediately stops all motor activity, bringing the vehicle to a halt.

⚙️ Technologies Used
ESP32 Microcontroller

MQTT Communication Protocol

ADC/DAC Conversion

L298N Motor Driver

Ultrasonic Distance Sensors

