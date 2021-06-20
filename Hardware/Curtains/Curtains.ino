//Mqtt Data
#include "EspMQTTClient.h"

EspMQTTClient client(
  "Slow Internet Here",
  "Superman!",
  "ashhomeassistantmqtt.duckdns.org",  // MQTT Broker server ip
  "homeassistant",
  "ahhah9Mio6Oingaeweithihohsh0ieGhai4cua0yi9Xah0ya4poY3aeC4ozei6el",
  "IR Module",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

// Motor pin definitions
#define in1 26
#define in2 27

// Reed switches for stopping
#define reed1  13 //Reed at motor 
#define reed2  14 //Reed at pully

bool stopFlag = false;

void moves(bool dir) {
  if (dir) { //Moves towards motor
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    while (digitalRead(reed1) == false) //reed1 is near the motor
    {
      if (stopFlag) {
        stopFlag = false;
        break;
      }
    }
    client.loop();
  }
  else { // Moves towards pully
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    while (digitalRead(reed2) == false) //reed2 is near the pully
    {
      if (stopFlag) {
        stopFlag = false;
        break;
      }
    }
    client.loop();
  }
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void onConnectionEstablished()
{
  client.subscribe("Curtains/open", [](const String & payload) {
    moves(true);
  });
  client.subscribe("Curtains/close", [](const String & payload) {
    moves(false);
  });
  client.subscribe("Curtains/stop", [](const String & payload) {
    stopFlag = true;
  });
}

void setup() {
  Serial.begin(115200);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(reed1, INPUT);
  pinMode(reed2, INPUT);

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater();
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");

  Serial.println("ESP is running!");
}

void loop() {
  client.loop();
}
