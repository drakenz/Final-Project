#include "EspMQTTClient.h"

EspMQTTClient client(
  "WE_B9EBA5",
  "j9501114",
  "192.168.1.187",  // MQTT Broker server ip
  "homeassistant",   // Can be omitted if not needed
  "few5thohPh6ohl8fahlazaKiwoocohchih3Puchoopoopheukaichiph1faquevo",   // Can be omitted if not needed
  "Motion Detector",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);
//int led_pin = 4;
//int pir_pin = 4;
// defines pins numbers
const int trigPinLe = 12;
const int echoPinLe = 14;

const int trigPinRi = 27;
const int echoPinRi = 26;

// defines variables
//long durationLe;
int distanceLe;
int distanceRi;


//bool myBool;
//bool left;
//int timer;

//bool person;
//bool oldPIR;
//bool currentPIR;
//bool PIR;
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
  //pinMode(pir_pin, INPUT);
}

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
  delay(10);
  distanceRi = readultra(0);

  /*
    currentPIR = digitalRead(pir_pin);
    if (!oldPIR && currentPIR) {
      PIR = true;
      righttime = 500;
    }
    oldPIR = currentPIR;
  */

  //Serial.print("dis. = ");
  //Serial.println(distanceLe);

  if (distanceLe < 50)
    lefttime = 25;
  if (distanceRi < 50)
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
  Serial.print(lefttime);
  Serial.print("\tRight : ");
  Serial.print(righttime);
  Serial.print("\t Humans :");
  Serial.println(nppl);

  /*
    person = false;
    for (int i = 0 ; i < 100; i++) {
      currentPIR = digitalRead(pir_pin);
      //Serial.println(currentPIR);
      if (!oldPIR && currentPIR) {
        person = true;
        break;
      }
      delay(25);
    }
    if (person) {
      client.publish("pir/state", "on");
      nppl++;
    }
    else {
      client.publish("pir/state", "off");
      nppl--;
    }
    }
    Serial.println(nppl);
  */client.loop();
}
