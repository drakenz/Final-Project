int sensorValue;
//int follower;
//int oldValue;

void setup() {
  Serial.begin(9600);
  
  //Long setup time waitting for the sensor to heat up to become operational
  while(true){
    sensorValue = analogRead(36);
    Serial.println(sensorValue);

    if(sensorValue < 550)
      break;

    delay(2000);
  }
  follower = sensorValue;
}

void loop() {
  sensorValue = analogRead(36);
  //follower -= (follower - sensorValue) / 10;
  
  /*//Smooth the sensor reading
  for(int i = 0 ; i < 20 ; i++){
    sensorValue += analogRead(36);
    sensorValue /= 2;
    delay(10);
  }*/

  //if((follower - sensorValue) > 100)
  //if((sensorValue - oldValue) > 100)
  if(sensorValue > 300)
    Serial.println("Gas Detected!");  

  Serial.println(sensorValue);
  //Serial.print(" ");
  //Serial.println(oldValue);

  //oldValue = sensorValue;
  delay(2000);
}
