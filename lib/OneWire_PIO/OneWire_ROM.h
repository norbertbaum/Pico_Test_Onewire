// Generic ROM commands for 1-Wire devices
// https://www.analog.com/en/technical-articles/guide-to-1wire-communication.html
// https://github.com/raspberrypi/pico-examples/blob/master/pio/onewire/ow_rom.h
#define OneWire_READ_ROM         0x33
#define OneWire_MATCH_ROM        0x55
#define OneWire_SKIP_ROM         0xCC
#define OneWire_ALARM_SEARCH     0xEC
#define OneWire_SEARCH_ROM       0xF0