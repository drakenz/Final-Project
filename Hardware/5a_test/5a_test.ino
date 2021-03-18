#include<EmonLib.h> // Include Emon Library
EnergyMonitor emon1; // Create an instance</p>
void setup() {
  Serial.begin(9600);
  emon1.current(0 , 9); // Current: input pin, calibration.
}
void loop() {
  double Irms = emon1.calcIrms(2000); // Calculate Irms only
  Serial.println(Irms); // Irms
}
