#include <SRAM2.h>

void setup()
{
Serial.begin(9600);   // initialize serial communications 
}//end of setup

void loop()
{
SRAM2.writestream1(22670);  //open writestream to write a byte to SRAM 1
SRAM2.RWdata(75); //the byte to write to SRAM at current address
SRAM2.readstream1(22670);   //open readstream to read a byte from SRAM 1
int data = SRAM2.RWdata(0xff);  //read byte from SRAM at current address
Serial.println(data);  //check data

SRAM2.writestream2(670);  //open writestream to write a byte to SRAM 2
SRAM2.RWdata(205); //the byte to write to SRAM at current address
SRAM2.readstream2(670);   //open readstream to read a byte from SRAM 2
data = SRAM2.RWdata(0xff);  //read byte from SRAM at current address
Serial.println(data);  //check data

delay(1000);
}//end of loop



