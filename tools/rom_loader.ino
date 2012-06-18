#include <Wire.h>

void setup() {
  delay(500);
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

void toggle_led() {
  static int state = HIGH;
  if (state == HIGH) {
    state = LOW;
    digitalWrite(13, LOW);
  } else {
    state = HIGH;
    digitalWrite(13, HIGH);
  }
}

void write_rom(int dev, unsigned int addr, byte val) {
  Wire.beginTransmission(dev);
  Wire.write(addr >> 8);
  Wire.write(addr & 0xff);
  Wire.write(val);
  Wire.endTransmission();
  delay(5);
  toggle_led();
}

byte read_rom(int dev, unsigned int addr) {
  Wire.beginTransmission(dev);
  Wire.write(addr >> 8);
  Wire.write(addr & 0xff);
  Wire.endTransmission(false);
  Wire.requestFrom(dev, 1);
  return Wire.read() & 0xff;
  toggle_led();
}

byte read_byte() {
  while (!Serial.available()) delay(50);
  return Serial.read();
}

/*
   PC sends the following over 9600 baud serial to write data to e2:

   [     1 byte     ] [     2 bytes    ] [   2 bytes   ] [  n bytes ]
   [ device address ] [ address offset ] [ data length ] [   data   ]
   
   PC then reads data_length bytes from serial and verifies these are the bytes that were sent
*/
void loop() {
  // Write ROM from serial
  int dev = read_byte();                     // e2 device address
  int addr = read_byte() << 8 | read_byte(); // address
  int len = read_byte() << 8 | read_byte();  // data length
  for (int i = addr; i < len; i++) write_rom(dev, i, read_byte());
  
  // Read ROM to serial (for verification)
  for (int i = addr; i < len; i++) Serial.write(read_rom(dev, i));
}
