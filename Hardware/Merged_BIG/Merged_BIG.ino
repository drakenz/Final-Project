//MQTT Data
#include "EspMQTTClient.h"
EspMQTTClient client(
  "Slow Internet Here",                                               //SSID
  "Superman!",                                                        //Password
  "192.168.1.10",                                                     //Broker IP
  "homeassistant",                                                    //Broker Username
  "ahhah9Mio6Oingaeweithihohsh0ieGhai4cua0yi9Xah0ya4poY3aeC4ozei6el", //Broker Password
  "Room Module 1",                                                    //Client Name
  1883                                                                //MQTT port
);

//Saved Prefrences
#include <Preferences.h>
Preferences preferences;

//IR Cloner data
#include "ESP32_IR_Remote.h"
ESP32_IRrecv irrecv;

//IR Pins
#define RECV_PIN 17
#define SEND_PIN 2

//Data Array
unsigned int IRData[11][500];
int codeLen[11];

//Huamn Counter
//Ultrasonic Pins
#define TRIGGER_LEFT_PIN 27
#define ECHO_LEFT_PIN 26

#define TRIGGER_RIGHT_PIN 14
#define ECHO_RIGHT_PIN 12

//Ultrasonic Values
int leftDistance;
int rightDistance;
int leftTime;
int rightTime;
int numberOfPeople;

//Switch
#define switchPin 25
int switchTime;
bool alreadySent = 0;

//Gas data
#define gasPIN 32
int gasValue;

void setup() {
  pinMode(TRIGGER_LEFT_PIN, OUTPUT);
  pinMode(ECHO_LEFT_PIN, INPUT);
  pinMode(TRIGGER_RIGHT_PIN, OUTPUT);
  pinMode(ECHO_RIGHT_PIN, INPUT);
  pinMode(switchPin, OUTPUT);

  pinMode(RECV_PIN, INPUT);
  pinMode(SEND_PIN, OUTPUT);

  Serial.begin(115200);
  delay(10);
  preferences.begin("IR", false);

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");

  loadData();
  Serial.println("ESP is running!");
}

void onConnectionEstablished()
{
  client.subscribe("IR/Init", [](const String & payload) {
    if (payload != "Done") {
      ReceiveIR(payload.toInt());
      client.publish("IR/Init", "Done");
      printArr();
    }
    Serial.println(payload);
  });
  client.subscribe("IR/Send", [](const String & payload) {
    SendIR(payload.toInt());
    Serial.println(payload);
  });
  client.subscribe("IR/Save", [](const String & payload) {
    saveData();
  });
  client.subscribe("IR/Load", [](const String & payload) {
    loadData();
  });
  client.subscribe("smorttv/payload", [](const String & payload) {
    //Serial.println(payload);
    if (payload == "0") {
      digitalWrite(switchPin, HIGH);//off
      client.publish("TVSwitch/State", "0");
      switchTime = 0;
    }
    else if (payload == "1") {
      digitalWrite(switchPin, LOW);//on
      client.publish("TVSwitch/State", "1");
      switchTime = 1000;
    }
  });
  /*
    client.subscribe("Human/State", [](const String & payload) {
    //Serial.println(payload);
    });
    client.subscribe("Human/Count", [](const String & payload) {
    //Serial.println(payload);
    });

    client.subscribe("TVSwitch/State", [](const String & payload) {
    //Serial.println(payload);
    });
  */
}

void ReceiveIR(int index) {
  Serial.println("Ready to receive!");
  irrecv.ESP32_IRrecvPIN(RECV_PIN, 0); //channel 0 so it can use the full memory of the channels
  irrecv.initReceive(); //setup the ESP32 to Receive IR code

  while (true) {
    codeLen[index] = 0;
    codeLen[index] = irrecv.readIR(IRData[index], sizeof(IRData[index]));
    if (codeLen[index] > 6) {
      break;
    }
  }
  Serial.print("Recieved data with RAW code length: ");
  Serial.println(codeLen[index]);

  Serial.print("At Index : ");
  Serial.println(index);
  irrecv.stopIR(); //uninstall the RMT channel so it can be reused for Receiving IR or send IR
}

void SendIR(int index) {
  Serial.println("Sending now!");

  irrecv.ESP32_IRsendPIN(SEND_PIN, 0); //channel 0 so it can use the full memory of the channels
  irrecv.initSend(); //setup the ESP32 to send IR code
  delay(1000);
  irrecv.sendIR(IRData[index], codeLen[index]);
  delay(1000);

  Serial.println("Data sent!");
  irrecv.stopIR(); //uninstall the RMT channel so it can be reused for Receiving IR or send IR
}

void printArr() {
  Serial.println("Codelen | Code");
  for (int i = 0 ; i < 11 ; i++)
  {
    Serial.print(codeLen[i]);
    Serial.print(" | ");

    for (int j = 0 ; j < codeLen[i] + 1; j++)
    {
      Serial.print(IRData[i][j]);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

void saveData()
{
  clearPrefs();
  int key = 0;
  char keyChar[16];
  String myString = "";

  for (int i = 0; i < 11; i++)
  {
    myString = ""; //Make a string of all Data (times) and save it as String
    myString.concat(String(codeLen[i]));
    myString.concat(" ");
    if (codeLen[i] != 0)
      for (int j = 0; j < codeLen[i] + 1; j++)
      {
        myString.concat(String(IRData[i][j]));
        myString.concat(" ");
      }
    itoa(key, keyChar, 10); //Convert Key to Char * to give to saved prefs
    preferences.putString(keyChar, myString);
    key++;
  }
  Serial.println("Data Saved!");
}

void loadData()
{
  int key = 0;
  char keyChar[16];
  String myString = "";
  for (int i = 0; i < 11; i++)
  {
    int j = 0;
    itoa(key, keyChar, 10);
    myString = preferences.getString(keyChar);

    int str_len = myString.length() + 1;
    char myStringChar[str_len];
    myString.toCharArray(myStringChar, str_len);

    char * token = strtok(myStringChar, " "); //Extract the first token
    codeLen[i] = strtoull(token, NULL, 0);
    if (codeLen[i] != 0) {
      token = strtok(NULL, " ");
      while ( token != NULL ) { // loop through the string to extract all other tokens
        IRData[i][j] = strtoull(token, NULL, 0);
        j++;

        token = strtok(NULL, " "); //Remove Left most value
      }
    }
    key++;
  }
  printArr();
  Serial.println("Data Loaded!");
}

void clearPrefs() {
  preferences.clear();
  Serial.println("Cleared!");
}

//Huamn Counter
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
  //Huamn Counter
h:
  leftDistance = readultra(1);
  delay(10);
  rightDistance = readultra(0);

  if (leftDistance < 50)
    leftTime = 80;
  if (rightDistance < 65)
    rightTime = 80;

  if (leftTime > 0 && rightTime > 0 && leftTime > rightTime) {
    numberOfPeople++;

    Serial.print("\t Humans :");
    Serial.println(numberOfPeople);

    client.publish("Human/State", "on");
    client.publish("Human/Count", String(numberOfPeople));
    digitalWrite(switchPin, LOW);
    client.publish("TVSwitch/State", "1");
    switchTime = 1000;//time applin. is on to make sure no one is coming back
    alreadySent = 0;
    leftTime = 0;
    rightTime = 0;
    goto h;
  }

  if (leftTime > 0 && rightTime > 0 && rightTime > leftTime) {
    if (numberOfPeople > 0) {
      numberOfPeople--;
      Serial.print("\t Humans :");
      Serial.println(numberOfPeople);
    }
    if (numberOfPeople == 0) {
      client.publish("Human/State", "off");
      client.publish("Human/Count", String(numberOfPeople));
    }
    leftTime = 0;
    rightTime = 0;
    goto  h;
  }

  if (leftTime > 0)
    leftTime--;
  if (rightTime > 0)
    rightTime--;
  if (switchTime > 0 && numberOfPeople == 0) {
    switchTime--;
  }
  if (switchTime == 0 && numberOfPeople == 0 && !alreadySent) {
    digitalWrite(switchPin, HIGH);
    client.publish("TVSwitch/State", "0");
    alreadySent = 1;
  }

  //Gas sensor
  gasValue = analogRead(gasPIN);

  if (gasValue > 300)
  {
    Serial.println("Gas Detected!");

    //Send to Server that we have gas here!
    client.publish("Mobile/Notification", "Gas Detected in Bedroom");
    client.publish("Emergency", "1");
  }
  client.loop();
}
