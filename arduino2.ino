#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <SoftwareSerial.h>

// rfid pins
#define ss_entry 10   // entry rfid sda
#define rst_entry 9   // entry rfid rst

#define ss_exit 8     // exit rfid sda
#define rst_exit 7    // exit rfid rst

// servo pins
#define entry_servo_pin 6
#define exit_servo_pin 5

// ESP8266 communication pins (RX, TX)
SoftwareSerial espSerial(2, 3); // RX on pin 2, TX on pin 3

// create rfid objects
MFRC522 rfid_entry(ss_entry, rst_entry);
MFRC522 rfid_exit(ss_exit, rst_exit);

// create two servos
Servo entry_servo;
Servo exit_servo;

// Array of registered RFID cards
byte auth_cards[5][7] = {
  {0x04, 0xB3, 0xAC, 0x53, 0x33, 0x02, 0x89}, // card 1
  {0x04, 0xA3, 0xEF, 0x4D, 0x33, 0x02, 0x89}, // card 2
  {0x04, 0xC3, 0x98, 0x54, 0x33, 0x02, 0x89}, // card 3
  {0x04, 0x13, 0xD5, 0x71, 0x33, 0x02, 0x89}, // card 4
  {0x04, 0x83, 0x06, 0x58, 0x33, 0x02, 0x89}  // card 5
};

// parking tracking
int total_slots = 4;
int available_slots = 4;
bool inside[5] = {false, false, false, false, false};

// time tracking (in milliseconds)
unsigned long entry_time[5] = {0, 0, 0, 0, 0};

// user count tracking
int total_entries = 0;

// Function to check which registered card matches the scanned UID
int get_card_index(MFRC522::Uid uid) {
  for (int c = 0; c < 5; c++) {
    bool match = true;
    for (int i = 0; i < uid.size; i++) {
      if (uid.uidByte[i] != auth_cards[c][i]) {
        match = false;
        break;
      }
    }
    if (match) return c;
  }
  return -1;
}

// Function to convert UID to a formatted string
String get_uid_string(MFRC522::Uid uid) {
  String uidStr = "";
  for (byte i = 0; i < uid.size; i++) {
    if (uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(uid.uidByte[i], HEX);
    if (i < uid.size - 1) uidStr += ":";
  }
  uidStr.toUpperCase();
  return uidStr;
}

// Function to send data to ESP8266 via serial
void send_to_esp(String command, String data) {
  espSerial.print(command);
  espSerial.print("|");
  espSerial.println(data);
  delay(100);
}

// Function to open the entry gate using servo
void open_entry_gate() {
  entry_servo.write(90);
  delay(2000);
  entry_servo.write(0);
}

// Function to open the exit gate using servo
void open_exit_gate() {
  exit_servo.write(90);
  delay(2000);
  exit_servo.write(0);
}

// Main function to process RFID reads for entry or exit
void check_reader(MFRC522 &reader, bool is_entry) {

  if (!reader.PICC_IsNewCardPresent()) return;
  if (!reader.PICC_ReadCardSerial()) return;

  // print uid
  Serial.print(is_entry ? "entry uid: " : "exit uid: ");
  String uidStr = get_uid_string(reader.uid);
  Serial.println(uidStr);

  int idx = get_card_index(reader.uid);

  if (idx >= 0) {

    if (is_entry) {
      // entry side
      if (!inside[idx]) {
        if (available_slots > 0) {
          Serial.println("entry granted (toll gate opens)");
          open_entry_gate();
          inside[idx] = true;
          available_slots--;

          // Record entry time
          entry_time[idx] = millis();

          // Increment total entries count and send to ThingSpeak
          total_entries++;

          // Send count to ThingSpeak via ESP8266
          send_to_esp("THINGSPEAK", String(total_entries));

          Serial.print("Total entries: ");
          Serial.println(total_entries);

        } else {
          Serial.println("parking full");
        }
      } else {
        Serial.println("car already inside");
      }
    } else {
      // exit side
      if (inside[idx]) {
        Serial.println("exit granted (toll gate opens)");
        open_exit_gate();

        // Calculate parking duration (simulate 1 min = 30 min, price $1 per 30 min)
        unsigned long duration_ms = millis() - entry_time[idx];
        unsigned long duration_minutes = duration_ms / 60000; // Convert to minutes
        unsigned long simulated_minutes = duration_minutes * 30; // 1 min = 30 min

        // Calculate dummy price ($1 per 30 minutes)
        float price = simulated_minutes * 1.0;

        // Send exit data to Telegram via ESP8266
        String telegram_data = uidStr + "," + String(simulated_minutes) + "," + String(price, 2);
        send_to_esp("TELEGRAM", telegram_data);

        Serial.print("Parked for: ");
        Serial.print(duration_minutes);
        Serial.print(" actual minutes (");
        Serial.print(simulated_minutes);
        Serial.println(" simulated minutes)");

        inside[idx] = false;
        available_slots++;
        entry_time[idx] = 0;

      } else {
        Serial.println("exit denied - car not inside");
      }
    }

  } else {
    Serial.println("unauthorized card");
  }

  reader.PICC_HaltA();
  reader.PCD_StopCrypto1();
  delay(300);
}

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600); // Communication with ESP8266
  SPI.begin();

  rfid_entry.PCD_Init();
  rfid_exit.PCD_Init();

  entry_servo.attach(entry_servo_pin);
  exit_servo.attach(exit_servo_pin);

  entry_servo.write(0);
  exit_servo.write(0);

  Serial.println("smart parking system started");
  Serial.print("available slots: ");
  Serial.println(available_slots);

  // Wait for ESP8266 to initialize
  delay(2000);
  Serial.println("waiting for esp8266...");
}

void loop() {
  check_reader(rfid_entry, true);
  check_reader(rfid_exit, false);

  // Check for messages from ESP8266
  if (espSerial.available()) {
    String response = espSerial.readStringUntil('\n');
    Serial.print("ESP: ");
    Serial.println(response);
  }
}
