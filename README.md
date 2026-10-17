# Smart Water Management & Cloud Logging System

An IoT-based water-level monitoring system using ESP32, an HC-SR04 ultrasonic sensor, ThingSpeak, and Google Sheets for cloud data logging.

## Overview

The Smart Water Management & Cloud Logging System is designed to monitor the water level in a storage tank using ultrasonic sensing. An ESP32 processes sensor readings, calculates the estimated water height, and transmits monitoring data over Wi-Fi to cloud platforms.

The system supports remote water-level monitoring and data logging, reducing the need for frequent manual inspection.

## Objectives

- Measure the distance between the sensor and the water surface without direct contact.
- Calculate the estimated water height and water-level percentage.
- Process sensor readings using the ESP32 microcontroller.
- Transmit monitoring data through Wi-Fi.
- Store and visualize water-level readings using ThingSpeak and Google Sheets.
- Provide a foundation for future water-level alerts and automatic pump control.

## System Architecture

**HC-SR04 Ultrasonic Sensor → ESP32 → Wi-Fi → Cloud Data Logging → Remote Monitoring**

The HC-SR04 ultrasonic sensor measures the distance between the sensor and the water surface. The ESP32 processes the sensor readings and calculates the estimated water height and percentage. Using a Wi-Fi connection, the ESP32 sends the monitoring data to ThingSpeak and Google Sheets. ThingSpeak supports cloud-based data visualization, while Google Sheets provides a record of readings through Google Apps Script.

## Hardware Components

- **ESP32 development board:** Processes sensor readings and handles Wi-Fi communication.
- **HC-SR04 ultrasonic sensor:** Measures the distance between the sensor and the water surface.
- **Voltage-divider resistors:** Reduce the HC-SR04 Echo signal voltage to a level suitable for the ESP32.
- **Breadboard and jumper wires:** Provide electrical connections between the components.
- **USB cable and power supply:** Supply power and support programming of the ESP32.
- **Water tank or container:** Holds the water being monitored.

## Software and Technologies

- **Programming language:** Embedded C/C++
- **Development environment:** Arduino IDE
- **Microcontroller:** ESP32
- **Sensor:** HC-SR04 ultrasonic sensor
- **Cloud platform:** ThingSpeak
- **Cloud spreadsheet:** Google Sheets
- **Integration:** Google Apps Script
- **Communication:** Wi-Fi and HTTP requests

## Circuit Connections

The HC-SR04 ultrasonic sensor is connected to the ESP32 as follows:

- **VCC:** Connected to the appropriate 5V supply supported by the development board.
- **GND:** Connected to ESP32 GND.
- **TRIG:** Connected to ESP32 GPIO 5.
- **ECHO:** Connected to ESP32 GPIO 18 through a suitable voltage divider.

The voltage divider is important because the HC-SR04 Echo output can be 5V, while ESP32 GPIO pins are not 5V tolerant.

## Working Principle

1. The ultrasonic sensor sends an ultrasonic pulse toward the water surface.
2. The sensor receives the reflected pulse and produces an Echo signal.
3. The ESP32 measures the Echo duration and calculates the distance to the water surface.
4. The program uses the configured tank height to estimate the water height.
5. The estimated water level is compared with the programmed thresholds to determine the water status.
6. The ESP32 sends the monitoring data through Wi-Fi to the configured cloud services.
7. The data can be viewed and recorded through ThingSpeak and Google Sheets.

## Water-Level Calculation

Let the configured tank height be `H` centimetres and the measured distance from the sensor to the water surface be `D` centimetres.

**Water Height (cm) = H − D**

The estimated water-level percentage is calculated as:

**Water Level (%) = ((H − D) / H) × 100**

The accuracy of these calculations depends on correct tank-height configuration and valid sensor measurements.

## Project Structure

- `SmartWater.ino` — Contains the ESP32 program for sensor measurement, water-level calculation, and cloud communication.
- `Images/` — Contains photographs of the circuit connections and hardware setup.
- `Demo/` — Contains the project output demonstration video.
- `README.md` — Provides project documentation, setup information, and the working principle.

## Demonstration

The `Images` folder contains photographs showing the hardware setup and circuit connections. The `Demo` folder contains a video demonstrating the project output and operation.

## Applications

- Residential water-tank monitoring.
- Water management in educational institutions.
- Agricultural water storage monitoring.
- Small-scale industrial water monitoring.
- IoT-based water-level monitoring systems.

## Limitations

- Measurement accuracy depends on sensor positioning and water-surface conditions.
- Cloud logging requires an internet connection and valid service configuration.
- The HC-SR04 is not a waterproof industrial sensor.
- Automatic pump control and alert features require additional implementation and testing.

## Future Enhancements

- Automatic water-pump control with suitable protection circuitry.
- Low-water and overflow alerts.
- Improved filtering of noisy sensor readings.
- Local data storage during network interruptions.
- A dedicated real-time monitoring dashboard.

## Author

**Shanthini G**  
Electronics and Communication Engineering
