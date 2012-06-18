#ifndef MMU_H
#define MMU_H

#include <I2C_eeprom.h>
#include <Arduino.h>

class MMU{
public:
	MMU() {};
	static void write(unsigned int addr, unsigned char val);
	static char read(unsigned int addr);
};

#endif
