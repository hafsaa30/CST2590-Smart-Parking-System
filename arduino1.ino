// Arduino 1 - ULTRASONIC, LEDs, LCD AND TEMP (CHANGES TO LCD)

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Assigning ultrasonic sensors pins
const int ultrasonicPins[4] = {2, 4, 7, 8};

// Assigning LED Pins [Red, Green]
const int ledPins[4][2] = {
  {3, 5},
  {6, 9},
  {10, 11},
  {12, 13}
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

  // Initialize LEDs - all green initially
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i][0], OUTPUT);
    pinMode(ledPins[i][1], OUTPUT);
    digitalWrite(ledPins[i][0], LOW);
    digitalWrite(ledPins[i][1], HIGH);
  }

  Serial.println("Arduino 1: Parking Monitor Active");
}


// Function to check all parking spots using ultrasonic sensors
void loop() {
  checkParkingSpots();
  updateDisplay();
  readTemperature();
  delay(500);
}

void checkParkingSpots() {
  availableSpots = 0;

  for (int i = 0; i < 4; i++) {
    float distance = getUltrasonicDistance(i);

    // Spot occupied if object within 30cm (and > 2cm to avoid noise)
    bool occupied = (distance > 2 && distance < 15);

    if (occupied != parkingStatus[i]) {
      parkingStatus[i] = occupied;
      updateLED(i, occupied);
    }

    if (!occupied) {
      availableSpots++;
    }
  }
}

// Function to measure distance using ultrasonic sensor
float getUltrasonicDistance(int sensorIndex) {
  int sensorPin = ultrasonicPins[sensorIndex];

  // Set pin as OUTPUT and send trigger pulse
  pinMode(sensorPin, OUTPUT);
  digitalWrite(sensorPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sensorPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorPin, LOW);

  // Set pin as INPUT and measure echo duration
  pinMode(sensorPin, INPUT);
  long duration = pulseIn(sensorPin, HIGH, 35000);

  if (duration == 0) {
    duration = 35000; // Timeout value
  }

  float distance = duration * 0.034 / 2;
  return distance;
}

// Function to update LCD display with available spots and temperature
void updateDisplay() {
  // First line: Available spots
  lcd.setCursor(0,0);
  if (availableSpots == 0) {
    lcd.print(" PARKING FULL!  ");
  } else {
    lcd.print("Slots Free: ");
    lcd.print(availableSpots);
    lcd.print("/4    "); // Extra spaces to clear previous text
  }

  // Second line: temperature
  lcd.setCursor(0,1);
  lcd.print("Temp:");
  lcd.print(" ");
  lcd.print((int)currentTemp);
  lcd.print("C");
  lcd.print("  ");

}

// Function to update LED for a specific parking spot
void updateLED(int spot, bool occupied) {
  if (occupied) {
    digitalWrite(ledPins[spot][0], HIGH);  // Red ON
    digitalWrite(ledPins[spot][1], LOW);   // Green OFF
  } else {
    digitalWrite(ledPins[spot][0], LOW);   // Red OFF
    digitalWrite(ledPins[spot][1], HIGH);  // Green ON
  }
}

// Function to read temperature from TMP36 sensor every 2 seconds
void readTemperature() {
  // Update temperature every 2 seconds
  if (millis() - lastTempUpdate >= 2000) {
    int sensorValue = analogRead(tmp36Pin);
    currentTemp = (sensorValue * 5.0 / 1024.0 - 0.5) * 100.0 - 5;
    lastTempUpdate = millis();
  }
}
