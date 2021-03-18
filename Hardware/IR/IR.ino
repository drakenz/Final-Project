#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

//First will create a decoder that handles only NEC, Sony and the hash
// decoder. If it is not NEC or Sony that it will return a 32 bit hash.
#include <stdlib.h>
//#include <IRLibSendBase.h>
//#include <IRLibDecodeBase.h>
//#include <IRLib_P01_NEC.h>
//#include <IRLib_P02_Sony.h>
//#include <IRLib_HashRaw.h>  //Must be last protocol
//#include <IRLibCombo.h>     // After all protocols, include this
// All of the above automatically creates a universal decoder
// class called "IRdecode" containing only the protocols you want.
// Now declare an instance of that decoder.
//IRdecode myDecoder;

// Include a receiver either this or IRLibRecvPCI or IRLibRecvLoop
//#include <IRLibRecv.h>
IRsend mySender;
IRrecv myReceiver(11);    //create a receiver on pin number 2

int IRPowerPin = 8;

String keys[11];
bool yy = false;
int i;
int s1;

void regkey() {
  String dd = "0x";
  if (myReceiver.getResults()) {
    myDecoder.decode();
    if (myDecoder.protocolNum == UNKNOWN) {
      //Serial.print(F("Unknown protocol. Hash value is: 0x"));
      //Serial.println(myDecoder.value);
    } else {
      myDecoder.dumpResults(false);
      Serial.print(myDecoder.value, HEX);
      Serial.print("  ,  ");
      Serial.println(myDecoder.value);
      dd.concat(String(myDecoder.value, HEX));
      keys[i] = dd;//it saves lowercases
      if (myDecoder.value == 4294967295) {
        i--;
      }
      yy = true;
    };
    myReceiver.enableIRIn();
  }
}

void setup() {
  pinMode(IRPowerPin, OUTPUT);
  digitalWrite(IRPowerPin, HIGH);

  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for Leonardo
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));

  for (i = 0; i < 1; i++) {
    if (i == 0) {
      Serial.println("Enter TV Power Button");
    }
    else {
      Serial.print("Enter Button No. ");
      Serial.print(i - 1);
      Serial.println(" : ");
    }
    while (1) {
      regkey();
      if (yy) {
        break;
      }
    }
    yy = false;
    delay(400);
  }

  digitalWrite(IRPowerPin, LOW);
}
uint32_t conv(String key) {
  int str_len = key.length() + 1;
  char x[str_len];
  key.toCharArray(x, str_len);
  Serial.println(x);
  uint32_t u = strtoul(x, NULL, 0);
  return u;
}

void loop() {
  while(Serial.available()==0){}
    int q = Serial.parseInt(); 
    Serial.println(q);
    uint32_t w = conv(keys[q]);
    Serial.print("we send : ");
    Serial.println(w);
    mySender.send(NEC, w, 32); //NEC TV power button=0x61a0f00f
  /*
    switch (s1){80BF3BC4
    case "on":
    mySender.send(NEC,keys[0],32);//NEC TV power button=0x61a0f00f
    break;
    case "off":
    mySender.send(NEC,keys[0],32);//NEC TV power button=0x61a0f00f
    break;
    case '0':
    mySender.send(NEC,keys[1],32);//NEC TV power button=0x61a0f00f
    break;
       case '1':
    mySender.send(NEC,keys[2],32);//NEC TV power button=0x61a0f00f
    break;
       case '2':
    mySender.send(NEC,keys[3],32);//NEC TV power button=0x61a0f00f
    break;
       case '3':
    mySender.send(NEC,keys[4],32);//NEC TV power button=0x61a0f00f
    break;
       case "'4':
    mySender.send(NEC,keys[5],32);//NEC TV power button=0x61a0f00f
    break;
       case "5":
    mySender.send(NEC,keys[6],32);//NEC TV power button=0x61a0f00f
    break;
       case "6":
    mySender.send(NEC,keys[7],32);//NEC TV power button=0x61a0f00f
    break;
       case "7":
    mySender.send(NEC,keys[8],32);//NEC TV power button=0x61a0f00f
    break;
       case "8":
    mySender.send(NEC,keys[9],32);//NEC TV power button=0x61a0f00f
    break;
       case "9":
    mySender.send(NEC,keys[10],32);//NEC TV power button=0x61a0f00f
    break;
    }*/
}
