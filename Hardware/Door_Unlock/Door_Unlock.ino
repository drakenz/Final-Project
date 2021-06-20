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

//Pins
const int Door_PIN = 26;
const int PIRPIN = 23;

//keypad Data
#include <ErriezTTP229.h>

// TTP229 pin defines
#define TTP229_SDO_PIN     16
#define TTP229_SCL_PIN     4

// Create keypad object
ErriezTTP229 ttp229;

ICACHE_RAM_ATTR

int pw = 1234;
int key;
int inpw;

int pir;
int prevpir;

int timeout = 200;
void keyChange()
{
  ttp229.keyChange = true;
}

void setup() {
  Serial.begin(115200);

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater();
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");

  ttp229.begin(TTP229_SCL_PIN, TTP229_SDO_PIN);
  attachInterrupt(digitalPinToInterrupt(TTP229_SDO_PIN), keyChange, FALLING);


  pinMode(Door_PIN, OUTPUT);
  pinMode(PIRPIN, INPUT);
  Serial.println("ESP is running!");
}

void onConnectionEstablished()
{
  client.subscribe("Door/State", [](const String & payload) {
    if (payload == "1") {
      openDoor();
    }
  });
}

void openDoor() {
  Serial.println("Opening Door");
  //client.publish("Door/State", "1");
  digitalWrite(Door_PIN, HIGH);

  delay(5000);

  client.publish("Door/State", "0");
  digitalWrite(Door_PIN, LOW);
}

void loop() {
  if (timeout > 0)
    if (--timeout == 0) {
      inpw = 0;
      Serial.println("TIMED OUT");
    }

  if (ttp229.keyChange) {
    timeout = 200;
    int key = ttp229.GetKey16();
    if (key != 0) {
      key -= 8;
      if (key > 9) {
        inpw = 0;
        delay(100); //for serial monitor only 3ashan bttb3 kteer
        Serial.print(key);
        Serial.println("Cleared!");
        return;
      }

      if (inpw != 0)
        inpw *= 10;
      inpw += key;

      Serial.print("Loop Prints : ");
      Serial.println(inpw);

      if (inpw == pw) {
        openDoor();
        inpw = 0;
        Serial.println("Unlocked");
      }

      else if (inpw > 999) {
        inpw = 0;
        Serial.println("Try again");
      }
    }
    ttp229.keyChange = false;
  }
  pir = digitalRead(PIRPIN);
  if (pir == 1 && prevpir == 0) {
    client.publish("Door/PIR", "1");
    Serial.print("PIR DETECTED MOVEMENT!");
  }
  prevpir = pir;
  delay(10);
  client.loop();
}
