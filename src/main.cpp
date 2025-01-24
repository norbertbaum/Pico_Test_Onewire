#include <Arduino.h>
#include "pico/time.h"
#include "hardware/adc.h"
#include <DS18B20.h>

// put function declarations here:
float getTemp();
void getTempDS27();
void getTempDS28();

uint32_t timeToRun = 3;
float temp;
DS18B20 *ds28;
DS18B20 *ds27;

void setup()
{
  // Serial.begin(115200); // Initialize Serial with a baud rate of 115200
  // while (!Serial)
  // {
  //   ; // Wait for Serial to connect (for native USB boards)
  // }

  ds27 = new DS18B20(pio0, 27);
  ds28 = new DS18B20(pio0, 28);

  Serial.println("Setup completed! Starting temperature readings...");
}

// void loop()
// {
//   // Serial.println("loop");

//   getTempDS27();
//   delay(1000);
//   getTempDS28();
//   // put your main code here, to run repeatedly:
// }

void loop()
{
  // Disable the state machine for ds28 while reading ds27
  // pio_sm_set_enabled(pio0, ds28->sm, false);
  getTempDS27();
  // pio_sm_set_enabled(pio0, ds28->sm, true);

  sleep_ms(500); // Small delay between sensors

  // Disable the state machine for ds27 while reading ds28
  // pio_sm_set_enabled(pio0, ds27->sm, false);
  getTempDS28();
  // pio_sm_set_enabled(pio0, ds27->sm, true);

  sleep_ms(2000); // Wait before next round
}

// void getTempDS27()
// {
//   Serial.println("Starting temperature conversion for DS18B20 - 27");
//   pio_sm_set_enabled(pio0, 1, false); // Disable SM 1 (ds28)

//   ds27->convert();
//   sleep_ms(1000);
//   temp = ds27->getTemperature();

//   if (temp == -2000)
//   {
//     Serial.println("Error: CRC check failed for DS18B20 - 27");
//   }
//   else
//   {
//     Serial.printf("DS18B20 - 27 Temp: %.2f°C\r\n", temp);
//   }

//   pio_sm_set_enabled(pio0, 1, true); // Re-enable SM 1
// }

// void getTempDS28()
// {
  
//   Serial.println("Starting temperature conversion for DS18B20 - 28");
//   pio_sm_set_enabled(pio0, 0, false); // Disable SM 0 (ds27)

//   ds28->convert();
//   sleep_ms(1000);
//   temp = ds28->getTemperature();

//   if (temp == -2000)
//   {
//     Serial.println("Error: CRC check failed for DS18B20 - 28");
//   }
//   else
//   {
//     Serial.printf("DS18B20 - 28 Temp: %.2f°C\r\n", temp);
//   }

//   pio_sm_set_enabled(pio0, 0, true); // Re-enable SM 0
// }

void getTempDS27() {
    ds27->DS18Initalize(pio0, 27); // Reinitialize the PIO
    ds27->convert();
    // sleep_ms(1000);
    temp = ds27->getTemperature();
    Serial.printf("DS18B20 - 27 Temp: %.2f°C\r\n", temp);
}

void getTempDS28() {
    ds28->DS18Initalize(pio0, 28); // Reinitialize the PIO
    ds28->convert();
    // sleep_ms(1000);
    temp = ds28->getTemperature();
    Serial.printf("DS18B20 - 28 Temp: %.2f°C\r\n", temp);
}
/*
void getTempDS27()
{
  // timeToRun = to_ms_since_boot(get_absolute_time());

  ds27->convert();
  // timeToRun = to_ms_since_boot(get_absolute_time()) - timeToRun;
  // Serial.printf("Covert time %d ms \n", timeToRun);

  sleep_ms(1500);

  // timeToRun = to_ms_since_boot(get_absolute_time());
  temp = ds27->getTemperature();
  // timeToRun = to_ms_since_boot(get_absolute_time()) - timeToRun;
  Serial.printf("DS18B20 - 27 Temp %f in %d ms\r\n", temp, timeToRun);

  // Serial.printf("Pico Temp was %f\n", getTemp());
  // sleep_ms(500);
}

void getTempDS28()
{
  // timeToRun = to_ms_since_boot(get_absolute_time());

  ds28->convert();
  // timeToRun = to_ms_since_boot(get_absolute_time()) - timeToRun;
  // Serial.printf("Covert time %d ms \n", timeToRun);

  sleep_ms(1500);

  // timeToRun = to_ms_since_boot(get_absolute_time());
  temp = ds28->getTemperature();
  timeToRun = 3; // to_ms_since_boot(get_absolute_time()) - timeToRun;
  Serial.printf("DS18B20 - 28 Temp %f in %d ms\r\n", temp, timeToRun);

  // Serial.printf("Pico Temp was %f\n", getTemp());
  // sleep_ms(500);
}
*/

float getTemp()
{
  const float conversion_factor = 3.3f / (1 << 12);
  adc_select_input(4);

  float v = (float)adc_read() * conversion_factor;
  float t = 27.0 - ((v - 0.706) / 0.001721);

  return t;
}