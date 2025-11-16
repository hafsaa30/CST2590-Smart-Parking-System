#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ultrasonic Sensors [TRIG, ECHO]
const int ultrasonicPins[4][2] = {
  {2, 3},   // Sensor 1
  {8, 9},   // Sensor 2
  {A0, A1}, // Sensor 3
  {A2, A4}  // Sensor 4
};

// RGB LED Pins [Red, Green]
const int ledPins[4][2] = {
  {12, 13}, // LED 1
  {10, 11}, // LED 2
  {6, 7},   // LED 3
  {4, 5}    // LED 4
};

// TMP36 Sensor
const int tmp36Pin = A3;

// Parking status
bool parkingStatus[4] = {false, false, false, false};
int availableSpots = 4;
unsigned long lastTempUpdate = 0;
float currentTemp = 0.0;

void setup() {
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Parking");
  delay(2000);
  
  // Initialize ultrasonic sensors
  for (int i = 0; i < 4; i++) {
    pinMode(ultrasonicPins[i][0], OUTPUT);
    pinMode(ultrasonicPins[i][1], INPUT);
  }
  
  // Initialize LEDs - all green initially
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i][0], OUTPUT);
    pinMode(ledPins[i][1], OUTPUT);
    digitalWrite(ledPins[i][0], LOW);
    digitalWrite(ledPins[i][1], HIGH);
  }
  
  Serial.println("Arduino 1: Parking Monitor Active");
}

void loop() {
  checkParkingSpots();
  updateDisplay();
  readTemperature();
  delay(500);
}

void checkParkingSpots() {
  availableSpots = 0;
  
  for (int i = 0; i < 4; i++) {
    long duration = getUltrasonicDistance(i);
    float distance = duration * 0.034 / 2;
    
    // Spot occupied if object within 30cm (and > 2cm to avoid noise)
    bool occupied = (distance > 2 && distance < 35);
    
    if (occupied != parkingStatus[i]) {
      parkingStatus[i] = occupied;
      updateLED(i, occupied);
      
      Serial.print("Spot ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(occupied ? "OCCUPIED" : "FREE");
    }
    
    if (!occupied) {
      availableSpots++;
    }
  }
}

long getUltrasonicDistance(int sensorIndex) {
  digitalWrite(ultrasonicPins[sensorIndex][0], LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicPins[sensorIndex][0], HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicPins[sensorIndex][0], LOW);
  
  //return pulseIn(ultrasonicPins[sensorIndex][1], HIGH, 30000);
  
  // Error handling for pulseIn
  long duration = pulseIn(ultrasonicPins[sensorIndex][1], HIGH, 35000);
  if (duration == 0) {
    duration = 35000; // Consistent timeout value
  }
  return duration;
}

void updateLED(int spot, bool occupied) {
  if (occupied) {
    digitalWrite(ledPins[spot][0], HIGH);  // Red ON
    digitalWrite(ledPins[spot][1], LOW);   // Green OFF
  } else {
    digitalWrite(ledPins[spot][0], LOW);   // Red OFF
    digitalWrite(ledPins[spot][1], HIGH);  // Green ON
  }
}

void updateDisplay() {
  // First line: Available spots
  lcd.setCursor(0, 0);
  lcd.print("Free: ");
  lcd.print(availableSpots);
  lcd.print("/4");
  
  // Temperature
  lcd.print(" ");
  lcd.print((int)currentTemp);
  lcd.print("C");
  lcd.print("  ");
  
  // Second line: Status
  lcd.setCursor(0, 1);
  if (availableSpots == 0) {
    lcd.print(" PARKING FULL!  ");
  } else if (availableSpots == 4) {
    lcd.print(" All Spots Free ");
  } else {
    lcd.print(" Park Available ");
  }
}

void readTemperature() {
  // Update temperature every 2 seconds
  if (millis() - lastTempUpdate >= 2000) {
    int sensorValue = analogRead(tmp36Pin);
    currentTemp = (sensorValue * 5.0 / 1024.0 - 0.5) * 100.0;
    lastTempUpdate = millis();
  }
}
