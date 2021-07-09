//Moisture Pins
#define SENSOR_PIN 35
#define RELAY_PIN 5

//Moisture variables
int minWaterPercent = 20;
int value;

void setup()
{
  Serial.begin(115200);

  pinMode(SENSOR_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  Serial.println("Starting!");
}

void loop()
{
  value = analogRead(SENSOR_PIN);
  Serial.print("Raw Value: ");
  Serial.print(value);

  value = map(value, 4095, 450, 0, 100);

  Serial.print("\t\tPercentage: ");
  Serial.print(value);
  Serial.println("%");

  /*
    //send to home ass value percentage
    //client.publish("Moist/percent", (String)value);
  */

  if (value < minWaterPercent)
  {
    digitalWrite(RELAY_PIN, LOW);
    delay(10000);
    digitalWrite(RELAY_PIN, HIGH);
  }

  delay(50);
}
