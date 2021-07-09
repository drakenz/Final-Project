//MQTT Data
#include "EspMQTTClient.h"
EspMQTTClient client(
  "Slow Internet Here",                                               //SSID
  "Superman!",                                                        //Password
  "192.168.1.10",                                                     //Broker IP
  "homeassistant",                                                    //Broker Username
  "ahhah9Mio6Oingaeweithihohsh0ieGhai4cua0yi9Xah0ya4poY3aeC4ozei6el", //Broker Password
  "Room Entry Module",                                                //Client Name
  1883                                                                //MQTT port
);

// Ultrasonic Pins
#define TRIGGER_LEFT_PIN 12
#define ECHO_LEFT_PIN 14

#define TRIGGER_RIGHT_PIN 27
#define ECHO_RIGHT_PIN 26

// Ultrasonic Values
int leftDistance;
int rightDistance;
int leftTime;
int rightTime;
int numberOfPeople;

void setup()
{
  Serial.begin(115200);

  pinMode(TRIGGER_LEFT_PIN, OUTPUT);
  pinMode(ECHO_LEFT_PIN, INPUT);
  pinMode(TRIGGER_RIGHT_PIN, OUTPUT);
  pinMode(ECHO_RIGHT_PIN, INPUT);

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}

void onConnectionEstablished()
{
  client.subscribe("pir/state", [](const String & payload) {
    Serial.println(payload);
  });
}


int readultra(bool whichone) { // 1 for left 0 for right
  if (whichone) {
    digitalWrite(TRIGGER_LEFT_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIGGER_LEFT_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_LEFT_PIN, LOW);

    int duration = pulseIn(ECHO_LEFT_PIN, HIGH);
    return duration * 0.017;
  }
  digitalWrite(TRIGGER_RIGHT_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER_RIGHT_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_RIGHT_PIN, LOW);

  int duration = pulseIn(ECHO_RIGHT_PIN, HIGH);
  return duration * 0.017;
}


void loop() {
  leftDistance = readultra(1);
  //delay(10);
  rightDistance = readultra(0);

  if (leftDistance < 80)
    leftTime = 25;
  if (rightDistance < 80)
    rightTime = 25;

  // Someone Entered
  if (leftTime > 0 && rightTime > 0 && leftTime > rightTime) {
    numberOfPeople++;
    client.publish("pir/state", "on");

    leftTime = 0;
    rightTime = 0;
  }

  // Someone Left
  if (leftTime > 0 && rightTime > 0 && rightTime > leftTime) {
    if (numberOfPeople > 0)
      numberOfPeople--;
    if (numberOfPeople == 0)
      client.publish("pir/state", "off");

    leftTime = 0;
    rightTime = 0;
  }

  // Time for entery detection
  if (leftTime > 0)
    leftTime--;
  if (rightTime > 0)
    rightTime--;

  Serial.print("Left : ");
  Serial.print(leftDistance);
  Serial.print("\tRight : ");
  Serial.print(rightDistance);
  Serial.print("\t Number Of People :");
  Serial.println(numberOfPeople);
  client.loop();
}
