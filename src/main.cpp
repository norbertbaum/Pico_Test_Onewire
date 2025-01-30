#include <Arduino.h>
#include "pico/time.h"
#include "hardware/adc.h"
#include <DS18B20.h>

// put function declarations here:

uint32_t timeToRun = 3;
float temp;

DS18B20 *sensors[16];

void setup()
{
  // Serial.begin(115200); // Initialize Serial with a baud rate of 115200
  // while (!Serial)
  // {
  //   ; // Wait for Serial to connect (for native USB boards)
  // }

  sensors[0] = new DS18B20(pio0, 28); 
  sensors[1] = new DS18B20(pio0, 27); 
  sensors[2] = new DS18B20(pio0, 26);  // NICHT angeschlossen!
  sensors[3] = new DS18B20(pio0, 25);  // NICHT angeschlossen!
  sensors[4] = new DS18B20(pio0, 24);  // NICHT angeschlossen!
  sensors[5] = new DS18B20(pio0, 23);  // NICHT angeschlossen!
  sensors[6] = new DS18B20(pio0, 22);  // NICHT angeschlossen!
  sensors[7] = new DS18B20(pio0, 21);  // NICHT angeschlossen!
  sensors[8] = new DS18B20(pio0, 20);  // NICHT angeschlossen!
  sensors[9] = new DS18B20(pio0, 19);  // NICHT angeschlossen!
  sensors[10] = new DS18B20(pio0, 18); // NICHT angeschlossen!
  sensors[11] = new DS18B20(pio0, 17); // NICHT angeschlossen!
  sensors[12] = new DS18B20(pio0, 16); // NICHT angeschlossen!
  sensors[13] = new DS18B20(pio0, 15); // NICHT angeschlossen!
  sensors[14] = new DS18B20(pio0, 14); // NICHT angeschlossen!
  sensors[15] = new DS18B20(pio0, 13); // NICHT angeschlossen!
  // sensors[2] = new DS18B20(pio0, 26);

  Serial.println("Setup completed! Starting temperature readings...");
}

void loop()
{
  for (size_t i = 0; i < 16; i++)
  {
    // sensors[i]->DS18Initalize(); // Reinitialize the PIO
    // sensors[i]->convert();
    temp = sensors[i]->getTemperature();

    // Serial.printf("DS18B20 - %d Temp: %.2f°C\r\n", i, temp);
  }

  delay(1000);
}