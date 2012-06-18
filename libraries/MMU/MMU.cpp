#include "MMU.h"

void __log(char *str)
{
	Serial.write(str);
}

void MMU::write(unsigned int addr, unsigned char val)
{
	// Write value to underlying memory addr regardless of mapping
	_ee.writeByte(addr, val);

	switch (addr >> 12)
	{
	case 0x0:
	case 0x1:
	case 0x2:
	case 0x3:
	case 0x4:
	case 0x5:
	case 0x6:
	case 0x7:
	case 0x8:
	case 0x9:
		if (addr == 0x0000) {
			__log("I/O Port DDR\n");
		} else if (addr == 0x0001) {
			__log("I/O Port DR\n");
		} else {
			__log("RAM BANK 1\n");
		}
		break;
	case 0xa:
	case 0xb:
			__log("BASIC ROM\n");
	case 0xc:
			__log("RAM BANK 2\n");
		break;
	case 0xd:
			__log("I/O, Char ROM\n");
			switch (addr >> 8)
			{
			case 0xd0:
			case 0xd1:
			case 0xd2:
			case 0xd3:
				__log("VIC Registers\n");
				break;
			case 0xd4:
			case 0xd5:
			case 0xd6:
			case 0xd7:
				__log("SID Registers\n");
				break;
			case 0xd8:
			case 0xd9:
			case 0xda:
			case 0xdb:
				__log("Color RAM\n");
				break;
			case 0xdc:
				__log("CIA 1\n");
				break;
			case 0xdd:
				__log("CIA 2\n");
				break;
			case 0xde:
				__log("I/O 1\n");
				break;
			case 0xdf:
				__log("I/O 2\n");
				break;
			default:
				__log("MMU error: Undef mem location in I/O, Char ROM!\n");
			}
		break;
	case 0xe:
	case 0xf:
			__log("KERNAL ROM\n");
		break;
	default:
		__log("MMU error: Undef mem location!\n");
	}
}

char MMU::read(unsigned int addr)
{
	char val = 0;

	return val;
}