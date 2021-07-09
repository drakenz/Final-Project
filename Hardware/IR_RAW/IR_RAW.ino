//MQTT Data
#include "EspMQTTClient.h"
EspMQTTClient client(
  "Slow Internet Here",                                               //SSID
  "Superman!",                                                        //Password
  "192.168.1.10",                                                     //Broker IP
  "homeassistant",                                                    //Broker Username
  "ahhah9Mio6Oingaeweithihohsh0ieGhai4cua0yi9Xah0ya4poY3aeC4ozei6el", //Broker Password
  "IR Module",                                                        //Client Name
  1883                                                                //MQTT port
);

//Saved Prefrences
#include <Preferences.h>
Preferences preferences;

//Remote data
#include "ESP32_IR_Remote.h"
ESP32_IRrecv irrecv;

//Pins
#define RECV_PIN 17
#define SEND_PIN 2

//Data Array
unsigned int IRData[11][500];
int codeLen[11];

void setup() {
  Serial.begin(115200);

  preferences.begin("IR", false);

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater();
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

void loop() {
  client.loop();
}
