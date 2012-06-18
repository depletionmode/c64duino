/*
writestream1: Setup SRAM 1 in sequential write mode starting from the passed address.
              Bytes can then be written one byte at a time using RWdata(byte data).
			  Each byte is stored in the next location and it wraps around 32767.

writestream2: Setup SRAM 2 in sequential write mode starting from the passed address.
              Bytes can then be written one byte at a time using RWdata(byte data).
			  Each byte is stored in the next location and it wraps around 32767.

readstream1: Setup SRAM 1 in sequential read mode starting from the passed address.
             Bytes can then be read one byte at a time using  byte RWdata(0).The passed data is irrelavent.
			 Each byte is read from the next location and it wraps around 32767.

readstream2: Setup SRAM 2 in sequential read mode starting from the passed address.
             Bytes can then be read one byte at a time using  byte RWdata(0).The passed data is irrelavent.
			 Each byte is read from the next location and it wraps around 32767.

RWdata:      Write or read a byte at any time from the selected SRAM (1 or 2 may be selected).
             If the writesteam is open the passed byte will be written to the current address.
			 If the readstream is open the byte from the current address will be returned.

closeRWstream: Use to close the  open read or write stream for both SRAM 1 and 2.
               Dont need when changing between read/write.
			   Close before using SPI somewhere else.

digital pin 13    SCK
digital pin 12    MISO
digital pin 11    MOSI
digital pin 10    SS for SRAM 1
digital pin 9     SS for SRAM 2
*/

#ifndef SRAM_h
#define SRAM_h

#define setupSPI SPCR = 0x50 //Master mode, MSB first, SCK phase low, SCK idle low, clock/4
#define setupDDRB DDRB |= 0x2e  //set  SCK(13) MOSI(11) and SS1 , SS2 as outputs 
#define selectSS1 PORTB &= ~0x04  //set the SS to 0 to select SRAM 1 
#define deselectSS1 PORTB |= 0x04  //set the SS to 1 to deselect SRAM 1 
#define selectSS2 PORTB &= ~0x02  //set the SS to 0 to select SRAM 2 
#define deselectSS2 PORTB |= 0x02  //set the SS to 1 to deselect SRAM 2 

#include "Arduino.h"

class SRAM2class
{
public:

SRAM2class();  //the constructor
void writestream1(int address);
void readstream1(int address);
void writestream2(int address);
void readstream2(int address);
void closeRWstream(void);
byte RWdata(byte data);

};//end of class SRAM2class

 extern SRAM2class SRAM2;

#endif
