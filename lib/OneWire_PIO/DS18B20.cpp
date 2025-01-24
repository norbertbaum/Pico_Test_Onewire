/*
 * DS18B20.cpp
 * Based on code from
 * Harry Fairhead (2021): "The Pico In C: A 1-Wire PIO Program"
 *  Created on: 25 Mar 2022
 *      Author: jondurrant
 */

/*
* Geändert Norbert Baum
* 23.01.2025
* ToDO:
- Methode die alles macht (Cleanup, etc)
- die letzten x Werte speichern und Mittelwert bilden
- Fehlerbehandlung
- Namespace?
- Tests mit mehreren Sensoren
- UnitTests?
*/

#include <Arduino.h>
#include "DS18B20.h"

DS18B20::DS18B20()
{
    // NOP
}

DS18B20::DS18B20(PIO p, uint8_t gp)
{
    this->sm = 0;
    this->pio = p;
    this->gpio = gp;
    
    DS18Initalize();
}

DS18B20::~DS18B20()
{
    // NOP
}

/***
 * Return CRC8 of the data
 */
uint8_t DS18B20::crc8(uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint8_t j;
    uint8_t temp;
    uint8_t databyte;
    uint8_t crc = 0;
    for (i = 0; i < len; i++)
    {
        databyte = data[i];
        for (j = 0; j < 8; j++)
        {
            temp = (crc ^ databyte) & 0x01;
            crc >>= 1;
            if (temp)
                crc ^= 0x8C;
            databyte >>= 1;
        }
    }
    return crc;
}

/***
 * Write bytes to PIO
 * @param bytes
 * @param len
 */
void DS18B20::writeBytes(uint8_t bytes[], int len)
{
    pio_sm_put_blocking(pio, sm, 250);
    pio_sm_put_blocking(pio, sm, len - 1);
    for (int i = 0; i < len; i++)
    {
        pio_sm_put_blocking(pio, sm, bytes[i]);
    }
}

/***
 * Read bytes
 * @param bytes
 * @param len
 */
void DS18B20::readBytes(uint8_t bytes[], int len)
{
    pio_sm_put_blocking(pio, sm, 0);
    pio_sm_put_blocking(pio, sm, len - 1);
    for (int i = 0; i < len; i++)
    {
        bytes[i] = pio_sm_get_blocking(pio, sm) >> 24;
    }
}

/***
 * Covert Temperature
 * Leave 1000ms before getting temperature
 */
void DS18B20::convert()
{
    uint8_t d[2] = {0xCC, 0x44};
    writeBytes(d, 2);
}

/***
 * Get term in Celsius
 * @return te,[eratire
 */
float DS18B20::getTemperature()
{
    // Serial.printf("getTemperature\n");
    // Serial.printf("SM: %d\n", sm);

    uint8_t d[2] = {0xCC, 0xBE};
    writeBytes(d, 2);
    uint8_t data[9];
    readBytes(data, 9);
    uint8_t crc = crc8(data, 9);
    if (crc != 0)
        return -2000;
    int t1 = data[0];
    int t2 = data[1];
    int16_t temp1 = (t2 << 8 | t1);
    volatile float temp = (float)temp1 / 16;
    return temp;
}

/***
 * Initialise the DS18B20
 * @param pio
 * @param gpio
 * @return
 */
// void DS18B20::DS18Initalize(PIO p, int gpio)
// {
//     pio = p;
//     gp = gpio;

//     // Initialize GPIO for the PIO
//     pio_gpio_init(pio, gpio);

//     // Add the DS18B20 program to the PIO instance
//     uint offset = pio_add_program(pio, &DS1820_program);

//     // Claim a unique state machine for this instance
//     sm = pio_claim_unused_sm(pio, true);
//     if (sm == -1)
//     {
//         Serial.printf("No available state machine for GPIO %d\n", gpio);
//         return;
//     }

//     // Configure the state machine
//     pio_sm_config c = DS1820_program_get_default_config(offset);
//     sm_config_set_clkdiv_int_frac(&c, 255, 0);
//     sm_config_set_set_pins(&c, gpio, 1); // Ensure this is specific to the GPIO
//     sm_config_set_out_pins(&c, gpio, 1);
//     sm_config_set_in_pins(&c, gpio);
//     sm_config_set_in_shift(&c, true, true, 8);

//     // Initialize and enable the state machine
//     pio_sm_init(pio, sm, offset, &c);
//     pio_sm_set_enabled(pio, sm, true);

//     Serial.printf("Initialized DS18B20 on GPIO %d with PIO %d and SM %d\n", gpio, pio == pio0 ? 0 : 1, sm);
// }

void DS18B20::DS18Initalize()
{
    // sm = sm_id; // Use the explicitly provided state machine

    pio_sm_unclaim(pio, sm);           // Free the state machine (if claimed earlier)
    pio_clear_instruction_memory(pio); // Clear the instruction memory

    pio_gpio_init(pio, gpio);

    uint offset = pio_add_program(pio, &DS1820_program);

    // Claim a unique state machine for this instance
    sm = pio_claim_unused_sm(pio, true);

    pio_sm_config c = DS1820_program_get_default_config(offset);
    sm_config_set_clkdiv_int_frac(&c, 255, 0);
    sm_config_set_set_pins(&c, gpio, 1);
    sm_config_set_out_pins(&c, gpio, 1);
    sm_config_set_in_pins(&c, gpio);
    sm_config_set_in_shift(&c, true, true, 8);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    // Serial.printf("Initialized DS18B20 on GPIO %d with SM %d\r\n", gpio, sm);
}
