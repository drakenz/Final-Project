const int Analog_channel_pin= 12;
float ADC_VALUE = 0;
float voltage_value = 0; 
void setup() 
{
Serial.begin(115200);
}
void loop() 
{
  
ADC_VALUE = analogRead(Analog_channel_pin);
//Serial.print("ADC VALUE = ");
//Serial.print(ADC_VALUE);
//delay(100);

voltage_value = (3.78/4095)*ADC_VALUE;//3.78
//current --> [reading-offset]*1000/(mV/Amp)
float offset = 1.64;
float current = (voltage_value-offset)/0.066;
if(abs(current)>0.136){
Serial.print("   Voltage = ");
Serial.print(voltage_value);
Serial.print("V  ");
Serial.print("Current = ");
Serial.print(current);
Serial.println(" Amp");
}
else{
  Serial.println("Zero Amp");}
delay(500);
}