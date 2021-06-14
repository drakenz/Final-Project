#include "ESP32_IR_Remote.h"
#include "EspMQTTClient.h"

//For clearing NVS
//#include <nvs_flash.h>
//#include <esp_wifi.h>

#include <Preferences.h>
Preferences preferences;

EspMQTTClient client(
  "Slow Internet Here",
  "Superman!",
  "192.168.1.19",  // MQTT Broker server ip
  "admin",
  "admin",
  "IR Module",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

//Pins and IR init
const int RECV_PIN = 17;
const int SEND_PIN = 2;

ESP32_IRrecv irrecv;

//Data Array
unsigned int IRData[11][500];
int codeLen[11];

//Reading data at INIT
bool readData = false;
int myIndex;

//Sending data with IR
bool sendData = false;
int sendIndex;

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
      readData = true;
      myIndex = payload.toInt();
    }
    Serial.println(payload);
  });
  client.subscribe("IR/Send", [](const String & payload) {
    sendData = true;
    sendIndex = payload.toInt();
    Serial.println(payload);
  });
  client.subscribe("IR/Save", [](const String & payload) {
    saveData();
  });
  client.subscribe("IR/Load", [](const String & payload) {
    loadData();
  });
  /*
    client.subscribe("IR/Print", [](const String & payload) {
    printArr();
    });
    client.subscribe("IR/Clear", [](const String & payload) {
    clearPrefs();
    });
  */
}

void ReceiveIR() {
  Serial.println("Ready to receive!");
  irrecv.ESP32_IRrecvPIN(RECV_PIN, 0); //channel 0 so it can use the full memory of the channels
  irrecv.initReceive(); //setup the ESP32 to Receive IR code

  while (true) {
    codeLen[myIndex] = 0;
    codeLen[myIndex] = irrecv.readIR(IRData[myIndex], sizeof(IRData[myIndex]));
    if (codeLen[myIndex] > 6) {
      break;
    }
  }
  Serial.print("Recieved data with RAW code length: ");
  Serial.println(codeLen[myIndex]);

  Serial.print("At Index : ");
  Serial.println(myIndex);
  irrecv.stopIR(); //uninstall the RMT channel so it can be reused for Receiving IR or send IR
}

void SendIR() {
  Serial.println("Sending now!");

  irrecv.ESP32_IRsendPIN(SEND_PIN, 0); //channel 0 so it can use the full memory of the channels
  irrecv.initSend(); //setup the ESP32 to send IR code
  delay(1000);
  irrecv.sendIR(IRData[sendIndex], codeLen[sendIndex]);
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
  //To clear the NVS partition (Not recommended)
  //nvs_flash_erase(); // erase the NVS partition and...
  //nvs_flash_init(); // initialize the NVS partition.
  //esp_wifi_clear_fast_connect(); //Wifi stores data in nvs

  //Remove all preferences under the opened namespace
  preferences.clear();
  Serial.println("Cleared!");
}

void loop() {
  if (readData) {
    readData = false;
    ReceiveIR();
    client.publish("IR/Init", "Done");
    printArr();
  }
  if (sendData) {
    sendData = false;
    SendIR();
  }
  client.loop();
}
