# 🏗️ Cement Mixer Monitoring System – Internship Project

This project presents a real-time monitoring solution for a cement mixer using two custom-designed IoT circuits and a local Flask server. The circuits measure angular velocity, linear acceleration, and current consumption, and transmit the data over Wi-Fi to be logged and analyzed locally.

---

## 📁 Repository Structure

SCM_Concrete/
├── 📁 Codes/ # All Arduino and Python code files
├── 📁 Data/ # Collected sensor data and logs
├── 📁 Documents/ # Project documentation files (PDFs)
├── 📁 Other/ # Additional Python scripts created during the internship
├── 📁 Tests/ # Experimental/test code for concept verification
└── README.md # This file

---

## 📂 Codes

This folder contains all the production-level code used in the final circuits and server:

| File | Description |
|------|-------------|
| `mpu6050_sensor.ino` | Arduino sketch for reading angular velocity and acceleration from MPU6050 and sending data to Flask server |
| `sct013_current_sensor.ino` | Arduino sketch for measuring current with SCT013 and sending to Flask server |
| `flask_server.py` | Python Flask server that receives, logs, and stores data from both ESP8266 boards |

---

## 📂 Data

This folder includes real-time sensor data collected during deployment and logs used for analysis.

| File | Description |
|------|-------------|
| `cement_mixer_analysis_data.csv` | Merged dataset of all sensor readings from MPU6050 and SCT013 |
| `data_log_sheet.txt` | Raw data logs used during testing and prototyping |

---

## 📂 Documents

This folder contains comprehensive documentation for the project, designed to ensure future maintainability and clarity.

| File | Description |
|------|-------------|
| `Operational_Document.pdf` | Describes physical setup, Wi-Fi config, Flask server deployment, power requirements, and procedures |
| `Circuit_Document.pdf` | Detailed explanation of both circuits, components used, and wiring logic |
| `Coding_Document.pdf` | Walkthrough of Arduino and Python code including logic, libraries, and data flow |
| `PCB_Design_Document.pdf` | Covers PCB design process using EasyEDA and ordering through JLCPCB |

---

## 📂 Other

This folder includes supporting Python scripts created during the internship for data analysis and automation. These scripts are not part of the core deployment but reflect initiative taken to explore insights and improve workflow efficiency.

| File | Description |
|------|-------------|
| `Mixer_Analyzer.py` | Analyzes the cement mixer data (`cement_mixer_analysis_data.csv`) using `matplotlib` and `pandas`. It generates visual plots (e.g., time series graphs of acceleration, angular velocity, current) and provides descriptive statistics like mean, standard deviation, min, max, etc. |
| `Project_Data-Segregator.py` | A utility script to process Excel workbooks containing multiple projects. It segregates entries into separate sheets within the same workbook based on city names — useful for organizing large datasets by location. |

---

## 📂 Tests

This folder contains experimental or proof-of-concept scripts used to validate ideas before applying them to the final project. These helped ensure reliable data transmission, LED state management, and cloud integration.

| File | Description |
|------|-------------|
| `led_logger.py` | A lightweight Flask server that receives LED status (ON/OFF) from the ESP8266 board over HTTP. Used to test local server reception before deploying the full data pipeline. |
| `led_test.ino` | Arduino sketch for ESP8266 that transmits LED status to the `led_logger.py` server. Served as a minimal test of communication and request structure. |
| `LED_Test_ESP8266_ThingSpeak.ino` | Sends LED status data from ESP8266 to the ThingSpeak IoT cloud platform. This test was used to evaluate external cloud dashboard integrations before choosing local logging with Flask. |

---
