#define ANALOG_CHANNEL_PIN 12

float ADC;
float Voltage;
float Current;
float Offset = 1.60;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  ADC = analogRead(ANALOG_CHANNEL_PIN);
  Voltage = (2.78 / 4095) * ADC;

  //current --> [reading-offset]*1000/(mV/Amp)
  Current = (Voltage - Offset) / 0.066;
  if (abs(Current) > 0.2)
  {
    Serial.print("   Voltage = ");
    Serial.print(Voltage);
    Serial.print("V  ");
    Serial.print("Current = ");
    Serial.print(Current);
    Serial.println(" Amp");
  }
  else
  {
    Serial.println("Zero Amp");
  }
  delay(500);
}
