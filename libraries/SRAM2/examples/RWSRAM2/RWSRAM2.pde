#include <SRAM2.h>

void setup()
{
Serial.begin(9600);   // initialize serial communications
}//end of setup

void loop()
{
SRAM2.writestream1(0);  // write to SRAM 1
unsigned long stopwatch = millis(); //start stopwatch
for(unsigned int i = 0; i < 32768; i++)
SRAM2.RWdata(0x55); //write to every SRAM 1 address 
Serial.print(millis() - stopwatch);
Serial.println("   ms to write full SRAM 1");
SRAM2.readstream1(0);   // start address from 0 
for(unsigned int i = 0; i < 32768; i++)
{
if(SRAM2.RWdata(0xFF) != 0x55)  //check every address in the SRAM1
{
Serial.print("error in SRAM 1 at location  ");
Serial.println(i);
break;
}//end of print error
if(i == 32767)
Serial.println("no errors for SRAM 1 in its 32768 bytes");
}//end of get byte
Serial.println(" ");

SRAM2.writestream2(0);  // write to SRAM 2
stopwatch = millis(); //start stopwatch
for(unsigned int i = 0; i < 32768; i++)
SRAM2.RWdata(0xaa); //write to every SRAM 2 address 
Serial.print(millis() - stopwatch);
Serial.println("   ms to write full SRAM 2");
SRAM2.readstream2(0);   // start address from 0 
for(unsigned int i = 0; i < 32768; i++)
{
if(SRAM2.RWdata(0xFF) != 0xaa)  //check every address in the SRAM2
{
Serial.print("error in SRAM 2 at location  ");
Serial.println(i);
break;
}//end of print error
if(i == 32767)
Serial.println("no errors for SRAM 2 in its 32768 bytes");
}//end of get byte
Serial.println(" ");

delay(1000);
}//end of loop



