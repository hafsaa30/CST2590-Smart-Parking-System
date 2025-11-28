# CST2590-Smart-Parking-System

A comprehensive IoT-based smart parking management system using Arduino, ESP8266, RFID, ultrasonic sensors, and cloud integration with ThingSpeak and Telegram notifications.

## 📋 Overview

This repository contains the complete implementation of a Smart Parking System designed for CST2590 coursework. This smart parking system monitors parking spot availability in real-time, manages entry/exit access via RFID authentication, tracks parking duration, calculates fees, and provides live updates through IoT platforms. The system uses two Arduino boards and an ESP8266 module for complete parking automation.

## 🎥 Project Demo

**YouTube Video:** [Smart Parking System Prototype Demo](YOUR_YOUTUBE_LINK_HERE)

## 📊 Live Data Monitoring

**ThingSpeak Channel:** [View Live Parking Data](https://thingspeak.mathworks.com/channels/3169454)
- Real-time entry count tracking
- Historical data visualization
- Field 1: Total parking entries

## 🗂️ Repository Structure

```
CST2590-Smart-Parking-System/
├── README.md
├── arduino1.ino    # Ultrasonic sensors, LEDs, LCD, Temperature
├── arduino2.ino    # RFID readers and servo gate control
└── esp8266.ino     # WiFi communication, ThingSpeak, Telegram
```

## ✨ Features

- **Real-time Parking Monitoring**: 4 parking spots monitored via ultrasonic sensors
- **RFID Access Control**: Secure entry/exit with authorized RFID cards
- **Automated Gate Control**: Servo-controlled entry and exit gates
- **Dynamic LED Indicators**: Red (occupied) / Green (available) status for each spot
- **LCD Display**: Shows available spots and ambient temperature
- **Temperature Monitoring**: TMP36 sensor tracks parking area temperature
- **Cloud Integration**: Live data updates to ThingSpeak
- **Telegram Notifications**: Automated parking fee notifications with duration and cost
- **Fee Calculation**: Automatic billing based on parking duration
- **Multi-card Support**: Supports up to 5 registered RFID cards

## 🛠️ Hardware Components

### Arduino 1 - Parking Monitor System
| Component | Quantity | Purpose |
|-----------|----------|---------|
| Arduino Uno | 1 | Main controller |
| HC-SR04 Ultrasonic Sensor | 4 | Distance measurement |
| Red LED | 4 | Occupied indicator |
| Green LED | 4 | Available indicator |
| 16x2 I2C LCD Display | 1 | Information display |
| TMP36 Temperature Sensor | 1 | Temperature monitoring |
| 220Ω Resistors | 8 | LED current limiting |

### Arduino 2 - Access Control System
| Component | Quantity | Purpose |
|-----------|----------|---------|
| Arduino Uno | 1 | Main controller |
| MFRC522 RFID Reader | 2 | Entry/Exit card scanning |
| Servo Motor (SG90) | 2 | Gate control |
| RFID Cards/Tags | 5 | User authentication |
| Jumper Wires | Multiple | Connections |

### Communication Module
| Component | Quantity | Purpose |
|-----------|----------|---------|
| ESP8266 (ESP-01) | 1 | WiFi connectivity |
| 3.3V | 1 | ESP8266 power |

## 📋 Pin Configuration

### Arduino 1 (Parking Monitor)

Note: Using 3-pin mode for Ultrasonic Sensors, therefore TRIG and ECHO will use one pin on Arduino
```
┌─────────────────────────────────────┐
│  Component          Pin Assignment  │
├─────────────────────────────────────┤
│  Ultrasonic Slot 1      Pin 2       │
│  Ultrasonic Slot 2      Pin 4       │
│  Ultrasonic Slot 3      Pin 7       │
│  Ultrasonic Slot 4      Pin 8       │
│  LED Red 1              Pin 3       │
│  LED Red 2              Pin 6       │
│  LED Red 3              Pin 10      │
│  LED Red 4              Pin 12      │
│  LED Green 1            Pin 5       │
│  LED Green 2            Pin 9       │
│  LED Green 3            Pin 11      │
│  LED Green 4            Pin 13      │
│  TMP36 Sensor           A3          │
│  LCD I2C SDA            SDA         │
│  LCD I2C SCL            SCL         │
└─────────────────────────────────────┘
```

### Arduino 2 (RFID & Gates)
```
┌─────────────────────────────────────┐
│  Component          Pin Assignment  │
├─────────────────────────────────────┤
│  Entry RFID SDA         Pin 10      │
│  Entry RFID RST         Pin 9       │
│  Exit RFID SDA          Pin 8       │
│  Exit RFID RST          Pin 7       │
│  SPI MOSI               Pin 11      │
│  SPI MISO               Pin 12      │
│  SPI SCK                Pin 13      │
│  Entry Servo            Pin 6       │
│  Exit Servo             Pin 5       │
├─────────────────────────────────────┤
│  ESP8266                            │
│  RX                     Pin 2       │
│  TX                     Pin 3       │
│  RST                    RESET       │
│  IO0                    GND         │ *for programming only
│  EN                     3.3V        │
│  3V3                    3.3V        │
│  GND                    GND         │
└─────────────────────────────────────┘
```

## 📦 Required Libraries

Install these libraries via Arduino IDE Library Manager:

| Library | Version | Purpose |
|---------|---------|---------|
| `MFRC522` | Latest | RFID reader communication |
| `Servo` | Latest | Servo motor control |
| `SoftwareSerial` | Built-in | Arduino-ESP8266 communication |
| `LiquidCrystal_I2C` | Latest | LCD display control |
| `Wire` | Built-in | I2C communication |
| `SPI` | Built-in | SPI communication for RFID |
| `ESP8266WiFi` | Latest | WiFi connectivity |
| `WiFiClientSecure` | Latest | Secure HTTPS connections |

## ⚙️ Configuration Guide

### 1. WiFi Setup (ESP8266)
Edit `esp8266.ino`:
```cpp
const char* ssid = "Your_WiFi_Name";          // Replace with your WiFi SSID
const char* password = "Your_WiFi_Password";   // Replace with your WiFi password
```

### 2. Telegram Bot Configuration
1. Open Telegram and search for [@BotFather](https://t.me/botfather)
2. Send `/newbot` and follow instructions to create your bot
3. Copy the **Bot Token** provided
4. Get your **Chat ID**:
   - Search for [@userinfobot](https://t.me/userinfobot)
   - Send any message to get your Chat ID
5. Update in `esp8266.ino`:
```cpp
String botToken = "1234567890:ABCdefGHIjklMNOpqrsTUVwxyz";  // Your bot token
String chatID = "1234567890";                                // Your chat ID
```

### 3. ThingSpeak Setup
1. Create account at [ThingSpeak.com](https://thingspeak.com)
2. Create a new channel
3. Add Field 1: "Total Entries"
4. Copy your **Write API Key**
5. Update in `esp8266.ino`:
```cpp
const char* thingspeakApiKey = "YOUR_API_KEY_HERE";
```

### 4. RFID Card Registration
Read your RFID card UIDs using the RFID reader, then update in `arduino2.ino`:
```cpp
byte auth_cards[5][7] = {
  {0x04, 0xB3, 0xAC, 0x53, 0x33, 0x02, 0x89}, // Card 1
  // Add your card UIDs here
};

## 🚀 Installation & Setup

1. **Clone the Repository**
   ```bash
   git clone https://github.com/hafsaa30/CST2590-Smart-Parking-System.git
   cd CST2590-Smart-Parking-System
   ```

2. **Install Required Libraries**
   - Open Arduino IDE
   - Go to Tools → Manage Libraries
   - Install all required libraries listed above

3. **Upload Code**
   - Upload `arduino1.ino` to Arduino 1
   - Upload `arduino2.ino` to Arduino 2
   - Upload `esp8266.ino` to ESP8266

4. **Hardware Assembly**
   - Connect components according to pin configuration
   - Ensure proper power supply for all modules
   - Connect Arduino 2 to ESP8266 via SoftwareSerial

5. **Configuration**
   - Update WiFi credentials in ESP8266 code
   - Configure Telegram bot token and chat ID
   - Add ThingSpeak API key
   - Register RFID card UIDs

## 📊 System Operation

### Entry Process
```
1. User approaches entry gate
2. User scans RFID card at entry reader
   ├─ Card authorized?
   │  ├─ Yes: Continue to step 3
   │  └─ No: Display "Unauthorized card" → End
3. Check if parking spots available
   ├─ Spots available?
   │  ├─ Yes: Continue to step 4
   │  └─ No: Display "Parking full" → End
4. Check if car already inside
   ├─ Already inside?
   │  ├─ Yes: Display "Car already inside" → End
   │  └─ No: Continue to step 5
5. Open entry gate (servo 90°)
6. Record entry time
7. Update: inside[card] = true
8. Decrement available_slots
9. Increment total_entries
10. Send total_entries to ThingSpeak
11. Close entry gate after 2 seconds
12. Display success message
```

### Exit Process
```
1. User approaches exit gate
2. User scans RFID card at exit reader
   ├─ Card authorized?
   │  ├─ Yes: Continue to step 3
   │  └─ No: Display "Unauthorized card" → End
3. Check if car is inside
   ├─ Inside?
   │  ├─ Yes: Continue to step 4
   │  └─ No: Display "Exit denied" → End
4. Calculate parking duration
   ├─ duration = current_time - entry_time
   ├─ simulated_duration = duration * 30
   └─ price = simulated_duration * $1.00
5. Open exit gate (servo 90°)
6. Send receipt to Telegram:
   ├─ Card ID: XX:XX:XX:XX:XX:XX:XX
   ├─ Duration: X minutes
   └─ Amount: $X.XX
7. Update: inside[card] = false
8. Increment available_slots
9. Reset entry_time[card] = 0
10. Close exit gate after 2 seconds
11. Display success message
```

### Real-Time Monitoring
```
Continuous Loop (Arduino 1):
├─ Scan all 4 ultrasonic sensors
├─ Measure distance for each spot
├─ Detect if object within 15cm
│  ├─ Object detected: Mark spot as occupied
│  └─ No object: Mark spot as available
├─ Update LED indicators
│  ├─ Occupied: Red LED ON, Green LED OFF
│  └─ Available: Red LED OFF, Green LED ON
├─ Update LCD display
│  ├─ Line 1: "Slots Free: X/4" or "PARKING FULL!"
│  └─ Line 2: "Temp: XXC"
└─ Read temperature every 2 seconds
```

## 📱 Notifications

### Telegram Message Format
```
Card ID: 04:B3:AC:53:33:02:89
You have parked for 0 minutes
Amount for parking is: $0.00
```

### ThingSpeak Data
- Field 1: Total number of entries
- Real-time graph updates
- Historical data tracking

## 👥 Project Team

| Name | MISIS | GitHub Profile | Role |
|------|------|----------------|------|
| Fathima Hafsa Shehan | M01033536 | [@hafsaa30](https://github.com/hafsaa30) | Team Leader |
| Amal Abdul Sattar | M01033486 | [@Raven2027](https://github.com/Raven2027) | Member |
| Faustine Franz | M01033532 | [@pouthewizard](https://github.com/pouthewizard) | Member |
| Aswathi Poyyillathu Reji | M01033483 | [@ash7531](https://github.com/ash7531) | Member |
| Geeth Gaudaman | M01033379 | [@geett-ioi](https://github.com/geett-ioi) | Member |

## ⚠️ Important Notes

### Security Considerations
- **Demo Purpose:** This system is designed for educational demonstration
- **Card Security:** RFID cards can be cloned; not suitable for high-security applications
- **Credentials:** Never commit actual API keys or passwords to public repositories
- **WiFi Security:** Use WPA2/WPA3 encrypted networks in production

## 📄 License

This project is submitted as coursework for CST2590.
All code and documentation are original work by the development team.
All external libraries and resources are properly attributed.

---

**Last Updated:** November 28, 2025  
**Repository Version:** 1.0  
**Status:** Active - CST2590 Coursework Submission

---

## Quick Reference

### GitHub Repository URLs
```
Main Repository:
https://github.com/hafsaa30/CST2590-Smart-Parking-System.git
```
