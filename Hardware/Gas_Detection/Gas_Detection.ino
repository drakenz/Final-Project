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

int sensorValue;

const int sensorPIN = 36;

void setup() {
  delay(1000);
  Serial.begin(115200);

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater();
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");

  //Long setup time waitting for the sensor to heat up to become operational
  while (true) {
    sensorValue = analogRead(sensorPIN);
    Serial.print("Sensor is heating up, Current value : ");
    Serial.println(sensorValue);

    if (sensorValue < 250)
      break;

    delay(10000);
  }
}
void onConnectionEstablished() {}

void loop() {
  sensorValue = analogRead(sensorPIN);


  if (sensorValue > 300) {
    Serial.println("Gas Detected!");

    //Send to Server that we have gas here!
    client.publish("Mobile/Notification", "Gas Detected in Bedroom");

  }

  Serial.print("Current Value : ");
  Serial.println(sensorValue);

  delay(5000);
  client.loop();
}
