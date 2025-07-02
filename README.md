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
