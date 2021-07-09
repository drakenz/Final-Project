#include "EspMQTTClient.h"

EspMQTTClient client(
  "Slow Internet Here",                                               //SSID
  "Superman!",                                                        //Password
  "192.168.1.10",                                                     //Broker IP
  "homeassistant",                                                    //Broker Username
  "ahhah9Mio6Oingaeweithihohsh0ieGhai4cua0yi9Xah0ya4poY3aeC4ozei6el", //Broker Password
  "Gas Module",                                                       //Client Name
  1883                                                                //MQTT port
);

#define sensorPIN 36;
int sensorValue;

void setup()
{
  delay(1000);
  Serial.begin(115200);

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater();
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");

  //Long setup time waitting for the sensor to heat up to become operational
  while (true)
  {
    sensorValue = analogRead(sensorPIN);
    Serial.print("Sensor is heating up, Current value : ");
    Serial.println(sensorValue);

    if (sensorValue < 250)
      break;

    delay(10000);
  }
}
void onConnectionEstablished() {}

void loop()
{
  sensorValue = analogRead(sensorPIN);

  if (sensorValue > 300)
  {
    Serial.println("Gas Detected!");

    //Send to Server that we have gas here!
    client.publish("Mobile/Notification", "Gas Detected in Bedroom");
    client.publish("Emergency", "1");    
  }

  Serial.print("Current Value : ");
  Serial.println(sensorValue);

  delay(5000);
  client.loop();
}
