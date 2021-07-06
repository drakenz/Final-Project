//OTA Import
#include "OTA.h"

//MQTT Data
#include "EspMQTTClient.h"

EspMQTTClient client(
  "Slow Internet Here",                                                   //SSID
  "Superman!",                                                            //Password
  "ashhomeassistantmqtt.duckdns.org",                                     //Broker IP
  "homeassistant",                                                        //Broker Username
  "ahhah9Mio6Oingaeweithihohsh0ieGhai4cua0yi9Xah0ya4poY3aeC4ozei6el",     //Broker Password
  "Door Module",                                                          //Client Name
  1883                                                                    //MQTT port
);

//Saved Preferences Data
#include <Preferences.h>
Preferences preferences;

//Pins
#define Door_PIN 26
#define PIR_PIN 25
#define REED_PIN 33

//Reed Data
int reedTimeout;
bool reed;

//keypad Data
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'c', '0', 'c'}
};
byte rowPins[ROWS] = {23, 22, 3, 21};
byte colPins[COLS] = {19, 18, 5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int key;
int password = 1234;
int currentPassword;
int keypadTimeout = 200;

//PIR data
bool currentPIR;
bool previousPIR;

void setup() {
  startOTA();
  Serial.begin(115200);

  pinMode(Door_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(REED_PIN, INPUT);

  preferences.begin("IR", false);
  password = preferences.getInt("Password");

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater();
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");

  Serial.println("ESP is running!");
  Serial.println(password);
}

void onConnectionEstablished()
{
  client.subscribe("Door/Open", [](const String & payload) {
    if (payload == "1")
      openDoor();
  });
  client.subscribe("Door/Password", [](const String & payload) {
    changePassword(payload.toInt());
  });
}

void openDoor() {
  Serial.println("Opening Door");
  client.publish("Door/State", "1");
  digitalWrite(Door_PIN, HIGH);

  delay(5000);

  client.publish("Door/State", "0");
  digitalWrite(Door_PIN, LOW);
}

void changePassword(int newPassword) {
  password = newPassword;
  preferences.putInt("Password", password);

  Serial.println(password);
}

void loop() {
  delay(1);
  //Check for keypad key pressed
  char key = keypad.getKey();
  if (key) {
    keypadTimeout = 200;
    if (key != 'c') {
      key = int(key) - 48;

      currentPassword *= 10;
      currentPassword += key;

      Serial.print("Loop Prints : ");
      Serial.println(currentPassword);

      if (currentPassword == password) {
        openDoor();
        currentPassword = 0;
        Serial.println("Unlocked");
      }
      if (currentPassword > 999)
        currentPassword = 0;
    }
    else {
      currentPassword = 0;
      Serial.println("Keypad Cleared");
    }
  }
  if (keypadTimeout > 0)
    if (--keypadTimeout == 0) {
      currentPassword = 0;
      Serial.println("Keypad Cleared");
    }

  //PIR Motion Detection
  currentPIR = digitalRead(PIR_PIN);
  if (currentPIR && !previousPIR) {
    client.publish("Door/PIR", "1");
    Serial.println("PIR DETECTED MOVEMENT!");
  }
  previousPIR = currentPIR;

  //Check if door left open with Reed
  reed = digitalRead(REED_PIN);
  if (reed)
    reedTimeout = 1000;

  if (reedTimeout > 0)
    if (--reedTimeout == 0) {
      Serial.println("Door Open");
      client.publish("Mobile/Notification", "You Forgot To Close the front door");
    }

  delay(10);
  server.handleClient();
  client.loop();
}
