# Smart-Energy-Meter-IoT
An ESP32-powered smart energy monitoring system that measures AC voltage, current, and power using ZMPT101B &amp; SCT013 sensors, displays data on LCD, controls loads via PIR-based relay switching, and uploads live readings to an online dashboard through HTTP.

# ⚡ ESP32 Smart Energy Monitor  
### Real-Time Voltage, Current, and Power Monitoring with IoT Dashboard & Motion-Based Load Control

![License](https://img.shields.io/badge/License-MIT-green.svg)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Status](https://img.shields.io/badge/Status-Working-brightgreen)
![IoT](https://img.shields.io/badge/IoT-Enabled-orange)

---

## 📌 Project Overview  
This project is a **Smart Energy Monitoring System** built using the **ESP32 microcontroller**.  
It measures:

- **AC Voltage** using ZMPT101B  
- **AC Current** using SCT-013 (100A/1V)  
- **Real Power (Watts)** using ADC sampling  
- **Relay Status** through PIR-based automation  

The system displays the data on a **16×2 I2C LCD** and also uploads it to a **live IoT dashboard** via HTTP using a custom backend.

Additionally, a **PIR motion sensor** automatically turns ON/OFF a relay-controlled AC load (bulb), making it useful for **smart automation and energy saving**.

---

## ✨ Features

### 🔹 Real-Time Electrical Monitoring  
- Measures **Voltage (Vrms)**  
- Measures **Current (Irms)**  
- Calculates **Real Power**  
- Calculates **Apparent Power (VA)**  
- High-speed ADC sampling (2000 samples per cycle)

### 🔹 Smart Automation with PIR  
- Turns ON relay when motion is detected  
- Automatically turns OFF after timeout  
- Perfect for home automation & energy saving  

### 🔹 IoT Cloud Connectivity  
- Sends data to an online server with HTTP POST  
- JSON-formatted IoT data  
- View real-time dashboard (Voltage, Current, Power, Relay Status)

### 🔹 Local Display  
- 16×2 LCD shows live electrical parameters  
- WiFi connection status displayed on boot

### 🔹 Safe Hardware Design  
- SCT-013 provides isolation  
- ZMPT101B provides voltage isolation  
- Relay safely operated via **BC548 transistor driver**

---

## 🛠 Hardware Used
| Component | Purpose |
|----------|---------|
| ESP32 Dev Module | Main controller & WiFi |
| ZMPT101B Module | AC Voltage measurement |
| SCT-013 CT Sensor (100A/1V) | AC Current measurement |
| PIR Motion Sensor | Motion detection |
| Relay Module (5V) | AC Load switching |
| BC548 Transistor | Relay driver |
| 10K & 1K resistors | Biasing & signal conditioning |
| 16×2 LCD (I2C) | Display output |
| AC Lamp Load | Demonstration load |

---

## 🔌 Wiring Diagram (Logical)

lua
Copy code
        +-------------------+
        |      ESP32        |
        |-------------------|
        SDA  -> GPIO21  ---- I2C LCD
        SCL  -> GPIO22  ---- I2C LCD
        ADC1 -> GPIO35  ---- ZMPT101B Voltage Out
        ADC2 -> GPIO34  ---- SCT-013 CT Sensor
        PIR  -> GPIO25  ---- PIR Output
        RELAY-> GPIO23  ---- BC548 Driver
        GND  -> Common Ground
        +5V  -> Relay + Sensors
        3.3V -> PIR Sensor (if required)
        +-------------------+
java
Copy code

Relay wiring (AC side):

AC Live ---> Relay COM
Relay NO ---> Bulb Live
Bulb Neutral ---> AC Neutral

yaml
Copy code

---

## 📸 Screenshots & Photos

### 🟦 Hardware Prototype  
*(Insert your photos here)*

### 🟩 Live Dashboard  
*(Insert your screenshot of website here)*

### 🟨 LCD Readings  
*(Insert your LCD picture here)*

---

## 📡 API / Backend Details  
The ESP32 sends HTTP POST requests to:

POST https://iot.om-mishra.com/data

bash
Copy code

### Example JSON payload:
```json
{
  "vrms": 204.10,
  "irms": 70.492,
  "power": 12241.20,
  "relay_state": true
}
💻 Software Flow (Working)
ESP32 boots

LCD: “WiFi Connecting…”

WiFi connected → show IP

PIR sensor checks motion

If motion: Relay ON

If no motion for 5 seconds: Relay OFF

ADC samples Vrms + Irms

Calculate power

Display data on LCD

Send data to server

Update cloud dashboard

Repeat loop

📥 How to Use This Project
1️⃣ Install Required Libraries
LiquidCrystal_I2C

Wire.h

HTTPClient.h

ESP32 board package

2️⃣ Upload the code using Arduino IDE
Select Board → ESP32 Dev Module

3️⃣ Connect the circuit as shown
4️⃣ Open Serial Monitor
You should see:

arduino
Copy code
WiFi Connecting...
WiFi Connected!
Sending data...
5️⃣ View Dashboard
Open your IoT dashboard to see live readings.

🧮 Mathematical Calculations
ini
Copy code
Vrms = √(Σ(v²) / N)
Irms = √(Σ(i²) / N)
P_real = Vrms × Irms × PowerFactor
Over 2000 samples are taken for accurate RMS values.

🧩 Future Improvements
Add energy consumption (kWh)

Add MQTT + Blynk support

Add mobile app dashboard

Add over-current protection relay

Integrate battery backup

PCB designing for compact build

👨‍💻 Author
Khushwant Maurya
B.Tech — Electronics & Communication Engineering
Smart IoT / Embedded Systems Developer

🔗 GitHub: https://github.com/Khushwanmaurya
🔗 LinkedIn: (Add your LinkedIn link)

📜 License
This project is released under the MIT License, allowing personal & commercial usage.

⭐ Support
If you like my project, don’t forget to star this repository ⭐ on GitHub!

yaml
Copy code
