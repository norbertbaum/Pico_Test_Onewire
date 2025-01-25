/*
 * DS18B20.cpp
 * Based on code from
 * Harry Fairhead (2021): "The Pico In C: A 1-Wire PIO Program"
 *  Created on: 25 Mar 2022
 *      Author: jondurrant
 * https://github.com/jondurrant/RP2040PIO-DS18B20
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
#include "DS18B20_ROM.h"
#include "OneWire_ROM.h"

DS18B20::DS18B20()
{
    // NOP
}

DS18B20::DS18B20(PIO p, uint8_t gp)
{
    this->sm = 0;
    this->pio = p;
    this->gpio = gp;

    // DS18Initalize();
}

DS18B20::~DS18B20()
{
    // NOP
    pio_sm_unclaim(this->pio, this->sm);     // Free the state machine (if claimed earlier)
    pio_clear_instruction_memory(this->pio); // Clear the instruction memory
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
    pio_sm_put_blocking(this->pio, this->sm, 250);
    pio_sm_put_blocking(this->pio, this->sm, len - 1);
    for (int i = 0; i < len; i++)
    {
        pio_sm_put_blocking(this->pio, this->sm, bytes[i]);
    }
}

/***
 * Read bytes
 * @param bytes
 * @param len
 */
void DS18B20::readBytes(uint8_t bytes[], int len)
{
    pio_sm_put_blocking(this->pio, this->sm, 0);
    pio_sm_put_blocking(this->pio, this->sm, len - 1);
    for (int i = 0; i < len; i++)
    {
        bytes[i] = pio_sm_get_blocking(this->pio, this->sm) >> 24;
    }
}

/***
 * Covert Temperature
 * Leave 1000ms before getting temperature
 */
void DS18B20::convert()
{
    uint8_t d[2] = {OneWire_SKIP_ROM, DS18B20_CONVERT_T};
    this->writeBytes(d, 2);
}

/***
 * Get term in Celsius
 * @return Temperature
 */
float DS18B20::getTemperature()
{
    if (this->isDeviceConnected())
    {
        this->DS18Initalize(); // Reinitialize the PIO
        this->convert();

        // Serial.printf("getTemperature\n");
        // Serial.printf("SM: %d\n", sm);

        uint8_t d[2] = {OneWire_SKIP_ROM, DS18B20_READ_SCRATCHPAD};
        this->writeBytes(d, 2);
        uint8_t data[9];
        this->readBytes(data, 9);
        uint8_t crc = DS18B20::crc8(data, 9);
        if (crc != 0)
            return -2000;
        int t1 = data[0];
        int t2 = data[1];
        int16_t temp1 = (t2 << 8 | t1);
        volatile float temp = (float)temp1 / 16;

        this->cleanup();

        return temp;
    }

    return -2000;
}

bool DS18B20::isDeviceConnected()
{
    gpio_init(this->gpio);    // GPIO initialisieren
    gpio_pull_up(this->gpio); // Aktiviert den internen Pull-Up-Widerstand

    gpio_set_dir(this->gpio, GPIO_OUT); // Als Ausgang setzen
    gpio_put(this->gpio, 0);            // LOW setzen
    sleep_us(480);                      // Reset-Signal senden
    gpio_set_dir(this->gpio, GPIO_IN);  // Als Eingang setzen, um auf Antwort zu warten
                                        // Auf "Presence Pulse" warten (15-60 µs High und dann 60-240 µs Low)
    sleep_us(70);                       // Warten auf den Presence-Puls (15-60 µs)

    bool presence = !gpio_get(this->gpio); // Wenn LOW erkannt wird, ist ein Gerät angeschlossen
                                           // sleep_us(410);                         // Sicherstellen, dass die gesamte Reset-Sequenz abgeschlossen ist
    gpio_pull_down(this->gpio);            // Deaktiviert den internen Pull-Up-Widerstand

    // Serial.printf("GPIO-Pin-Wert nach Reset: %d\n", gpio_get(this->gpio));
    // Serial.printf("Presence erkannt: %d\n", presence);

    return presence;
}

/***
 * Initialise the DS18B20
 * @return
 */
void DS18B20::DS18Initalize()
{
    this->cleanup();

    pio_gpio_init(this->pio, this->gpio);

    uint offset = pio_add_program(this->pio, &DS1820_program);

    // Claim a unique state machine for this instance
    this->sm = pio_claim_unused_sm(this->pio, true);

    pio_sm_config c = DS1820_program_get_default_config(offset);
    sm_config_set_clkdiv_int_frac(&c, 255, 0);
    sm_config_set_set_pins(&c, this->gpio, 1);
    sm_config_set_out_pins(&c, this->gpio, 1);
    sm_config_set_in_pins(&c, this->gpio);
    sm_config_set_in_shift(&c, true, true, 8);

    pio_sm_init(this->pio, this->sm, offset, &c);
    pio_sm_set_enabled(this->pio, this->sm, true);

    // Serial.printf("Initialized DS18B20 on GPIO %d with SM %d\r\n", gpio, sm);
}

void DS18B20::cleanup()
{
    /*
     * Aufräumen von sm und pio ist nötig, da sonst der PIO nicht mehr funktioniert
     * es gibt nur pio0 und pio1 - deswegen kann man nicht für 16 Sensoren jeweils einen eigenen PIO verwenden
     * SM (State Maschine, teil von PIO), gibt es auch nur bis zu 4? und deswegen muss auch die SM aufgeräumt/freigegeben werden
     *
     */
    pio_sm_unclaim(this->pio, this->sm);     // Free the state machine (if claimed earlier)
    pio_clear_instruction_memory(this->pio); // Clear the instruction memory
}