#include "ESP32_IR_Remote.h"

const int RECV_PIN = 17; // pin on the ESP32
const int SEND_PIN = 2; // pin on the ESP32


ESP32_IRrecv irrecv;

#define SendIRxTimes 1



unsigned int IRdata[1000]; //holding IR code in ms

void setup() {
  Serial.begin(115200);
  irrecv.ESP32_IRrecvPIN(RECV_PIN, 0); //channel 0 so it can use the full memory of the channels
  Serial.println("Initializing...");
  irrecv.initReceive();
  Serial.println("Init complete");
  Serial.println("Send an IR to Copy");
}


void loop() {
  int codeLen = 0;
  codeLen = irrecv.readIR(IRdata, sizeof(IRdata));
  if (codeLen > 6) { //ignore any short codes
    Serial.print("RAW code length: ");
    Serial.println(codeLen);
    irrecv.stopIR(); //uninstall the RMT channel so it can be reused for Receiving IR or send IR
    irrecv.ESP32_IRsendPIN(SEND_PIN, 0); //channel 0 so it can use the full memory of the channels
    irrecv.initSend(); //setup the ESP32 to send IR code
    delay(1000);
    for (int x = 0; x < SendIRxTimes; x++) { //send IR code that was recorded
      irrecv.sendIR(IRdata, codeLen);
      delay(1000);
    }
    codeLen = 0;
    irrecv.stopIR(); //uninstall the RMT channel so it can be reused for Receiving IR or send IR
    irrecv.ESP32_IRrecvPIN(RECV_PIN, 0); //channel 0 so it can use the full memory of the channels
    irrecv.initReceive(); //setup the ESP32 to Receive IR code
  }
}
