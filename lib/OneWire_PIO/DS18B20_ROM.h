// Function commands for d218b20 1-Wire temperature sensor 
// https://www.analog.com/en/products/ds18b20.html
// https://github.com/raspberrypi/pico-examples/blob/master/pio/onewire/ds18b20.h
#define DS18B20_CONVERT_T           0x44
#define DS18B20_WRITE_SCRATCHPAD    0x4E
#define DS18B20_READ_SCRATCHPAD     0xBE
#define DS18B20_COPY_SCRATCHPAD     0x48
#define DS18B20_RECALL_EE           0xB8
#define DS18B20_READ_POWER_SUPPLY   0xB4