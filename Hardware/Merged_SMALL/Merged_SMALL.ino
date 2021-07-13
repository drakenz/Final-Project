//Mqtt Data
#include "EspMQTTClient.h"

EspMQTTClient client(
  "Slow Internet Here",                                               //SSID
  "Superman!",                                                        //Password
  "192.168.1.10",                                                     //Broker IP
  "homeassistant",                                                    //Broker Username
  "ahhah9Mio6Oingaeweithihohsh0ieGhai4cua0yi9Xah0ya4poY3aeC4ozei6el", //Broker Password
  "Merged Module 2",                                                  //Client Name
  1883                                                                //MQTT port
);

// Motor pin definitions
#define in1 26
#define in2 27

// Reed switches for stopping
#define reed1 14 //Reed at motor
#define reed2 13 //Reed at pully

bool stopFlag = false;

//Moisture
#define MOISTURE_SENSOR_PIN 32
#define RELAY_PIN 25

//Moisture variables
int minWaterPercent = 20;
int value;

//ACS
#define ANALOG_CHANNEL_PIN 12

float ADC;
float Voltage;
float Current;
float Offset = 1.60;

void moveCurtain(bool dir)
{
  if (dir)
  { //Moves towards motor
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    while (digitalRead(reed1) == false) //reed1 is near the motor
    {
      client.loop();
      if (stopFlag)
      {
        stopFlag = false;
        break;
      }
    }
  }
  else
  { // Moves towards pully
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    while (digitalRead(reed2) == false) //reed2 is near the pully
    {
      client.loop();
      if (stopFlag)
      {
        stopFlag = false;
        break;
      }
    }
  }
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void onConnectionEstablished()
{
  client.subscribe("Curtains/open", [](const String & payload)
  {
    moveCurtain(true);
  });
  client.subscribe("Curtains/close", [](const String & payload)
  {
    moveCurtain(false);
  });
  client.subscribe("Curtains/stop", [](const String & payload)
  {
    stopFlag = true;
  });
}

void setup()
{
  Serial.begin(115200);
  digitalWrite(RELAY_PIN, HIGH);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(reed1, INPUT);
  pinMode(reed2, INPUT);

  pinMode(MOISTURE_SENSOR_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  pinMode(ANALOG_CHANNEL_PIN, INPUT);

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");

  Serial.println("ESP is running!");
}

void loop()
{
  //Moisture
  value = analogRead(MOISTURE_SENSOR_PIN);
  value = map(value, 4095, 450, 0, 100);




  /*
    //send to home ass value percentage
    //client.publish("Moist/percent", (String)value);
  */

  Serial.print("\t\tValue: ");
  Serial.print(value);

  Serial.print("\t\tMin: ");
  Serial.println(minWaterPercent);

  if (value < minWaterPercent)
  {
    digitalWrite(RELAY_PIN, LOW);
    delay(2000);
    digitalWrite(RELAY_PIN, HIGH);
  }

  //ACS
  ADC = analogRead(ANALOG_CHANNEL_PIN);
  Voltage = (2.78 / 4095) * ADC;

  //current --> [reading-offset]*1000/(mV/Amp)
  Current = (Voltage - Offset) / 0.066;
  if (abs(Current) > 0.2)
  {
    Serial.print("   Voltage = ");
    Serial.print(Voltage);
    Serial.print("V  ");
    Serial.print("Current = ");
    Serial.print(Current);
    Serial.println(" Amp");
  }
  else
  {
    Serial.println("Zero Amp");
  }
  client.loop();
}
