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

const int trigPinLe = 12;
const int echoPinLe = 14;

const int trigPinRi = 27;
const int echoPinRi = 26;

int distanceLe;
int distanceRi;

int nppl;
int lefttime;
int righttime;

void setup()
{
  pinMode(trigPinLe, OUTPUT);
  pinMode(echoPinLe, INPUT);
  pinMode(trigPinRi, OUTPUT);
  pinMode(echoPinRi, INPUT);
  Serial.begin(115200);

  // Optionnal functionnalities of EspMQTTClient :
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
  
}

void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("pir/state", [](const String & payload) {
    Serial.println(payload);
  });
}


int readultra(bool whichone) { // 1 for left 0 for right
  if (whichone) {
    digitalWrite(trigPinLe, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPinLe, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPinLe, LOW);

    int duration = pulseIn(echoPinLe, HIGH);
    return duration * 0.017;
  }
  digitalWrite(trigPinRi, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPinRi, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinRi, LOW);

  int duration = pulseIn(echoPinRi, HIGH);
  return duration * 0.017;
}


void loop() {
  distanceLe = readultra(1);
  //delay(10);
  distanceRi = readultra(0);

  if (distanceLe < 80)
    lefttime = 25;
  if (distanceRi < 80)
    righttime = 25;


  if (lefttime > 0 && righttime > 0 && lefttime > righttime) {
    nppl++;
    client.publish("pir/state", "on");

    lefttime = 0;
    righttime = 0;
  }

  if (lefttime > 0 && righttime > 0 && righttime > lefttime) {
    if (nppl > 0)
      nppl--;
    if (nppl == 0)
      client.publish("pir/state", "off");

    lefttime = 0;
    righttime = 0;
  }

  if (lefttime > 0)
    lefttime--;
  if (righttime > 0)
    righttime--;

  Serial.print("Left : ");
  Serial.print(distanceLe);
  Serial.print("\tRight : ");
  Serial.print(distanceRi);
  Serial.print("\t Humans :");
  Serial.println(nppl);
  client.loop();
}
