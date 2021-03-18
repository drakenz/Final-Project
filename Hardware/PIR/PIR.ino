// defines pins numbers
const int trigPinLe = 14;
const int echoPinLe = 27;
const int trigPinRi = 12;
const int echoPinRi = 13;

// defines variables
long durationLe;
int distanceLe;
long durationRi;
int distanceRi;
bool myBool;
bool left;
bool right;
int timer;

void setup() {
  pinMode(trigPinLe, OUTPUT);
  pinMode(echoPinLe, INPUT);
  pinMode(trigPinRi, OUTPUT);
  pinMode(echoPinRi, INPUT);
  Serial.begin(115200);
}

void loop() {
  // LEFT ultrasonic
  digitalWrite(trigPinLe, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPinLe, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinLe, LOW);

  durationLe = pulseIn(echoPinLe, HIGH);
  distanceLe = durationLe * 0.034 / 2;


  //Right ultrasonic
  digitalWrite(trigPinRi, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPinRi, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinRi, LOW);
  
  durationRi = pulseIn(echoPinRi, HIGH);
  distanceRi = durationRi * 0.034 / 2;

  /*
  // Prints the distance on the Serial Monitor
  Serial.print(distanceLe);
  Serial.print(' ');
  Serial.println(distanceRi);
 */

  if (distanceLe < 50 && !right) {
    left = true;
    timer = 100;
  }
  if (distanceRi < 50 && !left) {
    right = true;
    timer = 100;
  }
  if (timer-- == 0) {
    Serial.println("Out of time");
    left = false;
    right = false;
  }

  if (left && distanceRi < 50) {
      Serial.println("Left To right");
/*    while (distanceRi < 50) {
      //Right ultrasonic
      digitalWrite(trigPinRi, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPinRi, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPinRi, LOW);

      durationRi = pulseIn(echoPinRi, HIGH);
      distanceRi = durationRi * 0.034 / 2;
    }
*/
    left = false;
    right = false;
    delay(200);
    }
  if (right && distanceLe < 50) {
    Serial.println("Right To Left");
/*    while (distanceLe < 50) {
      digitalWrite(trigPinLe, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPinLe, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPinLe, LOW);

      durationLe = pulseIn(echoPinLe, HIGH);
      distanceLe = durationLe * 0.034 / 2;
    }
*/
    left = false;
    right = false;
    delay(200);
  }
}
