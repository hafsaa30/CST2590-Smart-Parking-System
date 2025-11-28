#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// WiFi Configuration
const char* ssid = "Wi-Fi Name "; //placeholder Wi-Fi name(Replace with your Wi-Fi name)
const char* password = "Wi-Fi Password"; //placeholder Wi-Fi Password(Replace with your Wi-Fi password)

// Telegram Bot Configuration
String botToken = "8461760620:AAH0-wrrEnMDbJGDluyBfUmtcRASSsV4n8w"; //Telegram bot token
String chatID = "6459639838"; //Chat ID where the bot will send the messages

// ThingSpeak Configuration
const char* thingspeakApiKey = "ZSC9MSLWX4ZFOO0C"; // API key for ThingSpeak
const char* thingspeakServer = "api.thingspeak.com"; //ThingSpeak server URL

WiFiClient client;
WiFiClientSecure clientSecure;

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  clientSecure.setInsecure();
  Serial.println("ESP8266 ready for parking data");
}

void loop() {
  // Check for data from Arduino
  if (Serial.available()) {
    String incoming = Serial.readStringUntil('\n');
    incoming.trim();

    // Parse command and data
    int separatorIndex = incoming.indexOf('|');
    if (separatorIndex > 0) {
      String command = incoming.substring(0, separatorIndex);
      String data = incoming.substring(separatorIndex + 1);

      if (command == "THINGSPEAK") {
        sendToThingSpeak(data.toInt());
      } else if (command == "TELEGRAM") {
        sendToTelegram(data);
      }
    }
  }

  delay(100);
}

// Function to send user count data to ThingSpeak
void sendToThingSpeak(int userCount) {
  Serial.println("Sending to ThingSpeak...");


  if (client.connect(thingspeakServer, 80)) {
    String url = "/update?api_key=" + String(thingspeakApiKey) +
                 "&field1=" + String(userCount);
// Send HTTP GET request to ThingSpeak
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + thingspeakServer + "\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("ThingSpeak update sent!");
    Serial.print("User count: ");
    Serial.println(userCount);

    delay(1000);
    client.stop();

  } else {
    Serial.println("ThingSpeak connection failed");
  }
}

//Function to send parking data as a message in Telegram
void sendToTelegram(String data) {
  Serial.println("Sending to Telegram...");

  // Parse data: UID,minutes,price
  int comma1 = data.indexOf(',');
  int comma2 = data.indexOf(',', comma1 + 1);

  String uid = data.substring(0, comma1);
  String minutes = data.substring(comma1 + 1, comma2);
  String price = data.substring(comma2 + 1);

  // Create message with URL encoding
  String message = "Card%20ID:%20" + urlEncode(uid) + "%0A";
  message += "You%20have%20parked%20for%20" + minutes + "%20minutes%0A";
  message += "Amount%20for%20parking%20is:%20$" + price;

  if (clientSecure.connect("api.telegram.org", 443)) {
    String url = "/bot" + botToken + "/sendMessage?chat_id=" + chatID + "&text=" + message;

    clientSecure.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: api.telegram.org\r\n" +
                "Connection: close\r\n\r\n");

    Serial.println("Telegram message sent!");
    Serial.println("UID: " + uid);
    Serial.println("Minutes: " + minutes);
    Serial.println("Price: $" + price);

    delay(2000);
    clientSecure.stop();

  } else {
    Serial.println("Telegram connection failed");
  }
}

//Helper function to URL-encode a string for safe transmission in URLs
String urlEncode(String str) {
  String encoded = "";
  char c;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ':') {
      encoded += "%3A";w
    } else if (c == ' ') {
      encoded += "%20";
    } else if (isalnum(c)) {
      encoded += c;
    } else {
      encoded += '%';
      encoded += String(c, HEX);
    }
  }
  return encoded;
}
