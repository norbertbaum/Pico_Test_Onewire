/*
 * DS18B20.h
 * Based on code from
 * Harry Fairhead (2021): "The Pico In C: A 1-Wire PIO Program"
 *  Created on: 25 Mar 2022
 *      Author: jondurrant
 */



#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "DS1820.pio.h"
#include <stdint.h>

class DS18B20 {
public:
	/***
	 * Constructor, does not initialse and DS18Initialize must also be called
	 */
	DS18B20();

	/***
	 * Constructure auto initialises
	 * @param p
	 * @param gp
	 */
	DS18B20(PIO p, uint8_t gp);

	virtual ~DS18B20();

	/***
	 * Get term in Celsius. Covert should be called 1000ms before	 
	 * @return
	 */
	float getTemperature();

private:
	/***
	 * Read bytes
	 * @param bytes
	 * @param len
	 */
	void readBytes(uint8_t bytes[],  int len);

	/***
	 * Write bytes to PIO
	 * @param bytes
	 * @param len
	 */
	void writeBytes(uint8_t bytes[], int len);

	/***
	 * Return CRC8 of the data
	 */
	static uint8_t crc8(uint8_t *data, uint8_t len);

		/***
	 * Initialise the DS18B20
	//  * @param pio
	//  * @param gpio
	 * @return
	 */
	// void DS18Initalize(PIO p, int gpio);
	void DS18Initalize();

	float readTemperature();
	bool isDeviceConnected();
	void cleanup();

	/***
	 * Covert Temperature
	 * Leave 1000ms before getting temperature
	 */
	void convert();

	PIO pio;	
	uint8_t gpio;
	uint sm;
};
