#include <Arduino.h>
/*
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
*/
const int sensor_pin = 35;
const int relay_pin = 5;

int minWaterPercent = 20;
int value;

/*
  void callback(char* topic, byte* payload, unsigned int length) {
  String response;
  for (int i = 0; i < length; i++) {
    response += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(response);
  if(response == "0")  // Turn the light on
  {
    digitalWrite(led_pin, HIGH);
    client.publish("Smortled/state","on");
  }
  else if(response == "100")  // Turn the light off
  {
    digitalWrite(led_pin, LOW);
    client.publish("Smortled/state","off");
  }
  }*/

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
/*void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("Moist/percent", [](const String &payload) {
    Serial.println(payload);
  });
}
*/
void setup()
{
  Serial.begin(9600);
  delay(2000);

  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, HIGH);
  
  Serial.println("Starting!");  
  //Optionnal functionnalities of EspMQTTClient :
  //client.enableDebuggingMessages();                                          // Enable debugging messages sent to serial output
  //client.enableHTTPWebUpdater();                                             // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  //client.enableLastWillMessage("TestClient/lastwill", "I am going offline"); // You can activate the retain flag by setting the third parameter to true
}

void loop()
{
  value = analogRead(sensor_pin);
  Serial.print("Raw Value: ");
  Serial.print(value);
  
  value = map(value, 4095, 450, 0, 100);

  Serial.print("\t\tPercentage: ");
  Serial.print(value);
  Serial.println("%");

  /*
  //send to home ass value percentage
  //client.publish("Moist/percent", (String)value);   
  */
  
  if (value < minWaterPercent)
  {
    digitalWrite(relay_pin, LOW);
    delay(10000);
    digitalWrite(relay_pin, HIGH);
  }
  
  delay(50);
}
