#include <Arduino.h>
#include "pico/time.h"
#include "hardware/adc.h"
#include <DS18B20.h>

// put function declarations here:


uint32_t timeToRun = 3;
float temp;
DS18B20 *ds28;
DS18B20 *ds27;

DS18B20* sensors[16];

void setup()
{
  // Serial.begin(115200); // Initialize Serial with a baud rate of 115200
  // while (!Serial)
  // {
  //   ; // Wait for Serial to connect (for native USB boards)
  // }

  ds27 = new DS18B20(pio0, 27);
  ds28 = new DS18B20(pio0, 28);  
  sensors[0] = ds27;
  sensors[1] = ds28;
  sensors[2] =  new DS18B20(pio0, 26); // NICHT angeschlossen!
  // sensors[2] = new DS18B20(pio0, 26);

  Serial.println("Setup completed! Starting temperature readings...");
}


void loop()
{
  for (size_t i = 0; i < 3; i++)
  {
    // sensors[i]->DS18Initalize(); // Reinitialize the PIO
    // sensors[i]->convert();
    temp = sensors[i]->getTemperature();

    Serial.printf("DS18B20 - %d Temp: %.2f°C\r\n", i,  temp);
  }

  delay(1000);
}