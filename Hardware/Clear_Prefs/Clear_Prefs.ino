#include <Preferences.h>
Preferences preferences;

#include <nvs_flash.h>
#include <esp_wifi.h>
void setup()
{
  //Clear saved prefrences
  preferences.clear();

  //Clear NVS
  nvs_flash_erase(); // erase the NVS partition
  nvs_flash_init(); // initialize the NVS partition.
  esp_wifi_clear_fast_connect(); // Clear wifi data in nvs

  Serial.println("Cleared!");
}

void loop() {}
