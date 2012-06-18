#include <Wire.h>
#include <SRAM2.h>
#include <Cpu_6502.h>

void setup() {
  delay(500);
  Serial.begin(9600);
  //pinMode(13, OUTPUT);
  //digitalWrite(13, HIGH);
  Serial.write("6502 library test\n");
}

unsigned long time;

void loop() {
  Cpu_6502 cpu(0x50);

  /* write test program to ROM @ $f000 */
  /*
  cpu.writeRom(0xf000, 0xa5);  // LDA $FF
  cpu.writeRom(0xf001, 0xff);
  cpu.writeRom(0xf002, 0xc9);  // CMP #$00
  cpu.writeRom(0xf003, 0x00);
  cpu.writeRom(0xf004, 0xf0);  // BEQ ($0010)
  cpu.writeRom(0xf005, 0xfa);
  cpu.writeRom(0xf006, 0x85);  // STA  $FD
  cpu.writeRom(0xf007, 0xfd);
  cpu.writeRom(0xf008, 0x4c);  // JMP $0010
  cpu.writeRom(0xf009, 0x10);
  cpu.writeRom(0xf00A, 0x00);
  */
  
  /* load test program from ROM $f000 into RAM */
  Serial.println("Loading test program from ROM $f000 into RAM $0010\n...");
  for (int i = 0; i < 0xb; i++) {
    byte val = cpu.readRom(0xf000 + i);
    Serial.println(val, HEX);
    cpu.writeRam(0x10 + i, val);
  }
  Serial.write("Done\n");
  
  Serial.write("Executing test program...\n");  
  cpu.reset(0x10);
  
  time = micros();
  while (1) {
    cpu.execIns();
    //Serial.println(micros() - time);
    //time = micros();
  }
}
