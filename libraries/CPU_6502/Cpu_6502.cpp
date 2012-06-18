#include <SRAM2.h>
#include <Wire.h>

#define _dbg(X)
//#define _dbg(X) Serial.write(X); delay(200); Serial.write("\r\n")

#include "Cpu_6502.h"

// TODO: refactor
void Cpu_6502::jump_branch(int offset ) {
  if( offset > 0x7f )
    _regPC = (_regPC - (0x100 - offset));
  else
    _regPC = (_regPC + offset );
}

// TODO: refactor
void Cpu_6502::ins_ADC(int value ) {
  int tmp;
  if( (_regA ^ value) & 0x80 ) {
    _regP &= 0xbf;
  } else {
    _regP |= 0x40;
  }

  if( _regP & 8 ) {
    tmp = (_regA & 0xf) + (value & 0xf) + (_regP&1);
    if( tmp >= 10 ) {
      tmp = 0x10 | ((tmp+6)&0xf);
    }
    tmp += (_regA & 0xf0) + (value & 0xf0);
    if( tmp >= 160) {
      _regP |= 1;
      if( (_regP&0xbf) && tmp >= 0x180 ) _regP &= 0xbf;
      tmp += 0x60;
    } else {
      _regP &= 0xfe;
      if( (_regP&0xbf) && tmp<0x80 ) _regP &= 0xbf;
    }
  } else {
    tmp = _regA + value + (_regP&1);
    if( tmp >= 0x100 ) {
      _regP |= 1;
      if( (_regP&0xbf) && tmp>=0x180) _regP &= 0xbf;
    } else {
      _regP &= 0xfe;
      if( (_regP&0xbf) && tmp<0x80) _regP &= 0xbf;
    }
  }
  _regA = tmp & 0xff;
  if( _regA ) _regP &= 0xfd; else _regP |= 0x02;
  if( _regA & 0x80 ) _regP |= 0x80; else _regP &= 0x7f;
}

void Cpu_6502::stack_push(byte val) {
  if (_regS >= 0) {
    _regS--;
    writeRam(_regS + 0x100, val);
  } 
}

byte Cpu_6502::stack_pop() {
  if (_regS < 0x100) {
    byte val = readRam(_regS++ + 0x100);
    return(val);
  }
  
  return(0);
}

void Cpu_6502::ins_AND(int val) {
  _regA &= val;
  if( _regA ) _regP &= 0xfd; else _regP |= 0x02;
  if( _regA & 0x80 ) _regP |= 0x80; else _regP &= 0x7f;
}

void Cpu_6502::ins_BIT(int val) {
  if (val & _regA) _regP &= 0xfd; else _regP |= 0x02;
  _regP = (_regP & 0x3f) | (val & 0xc0);
}

void Cpu_6502::ins_CMP(int _reg, int val) {
  if (_reg >= val) _regP |= 1; else _regP &= 0xfe;
  val = (_reg - val);
  if (val) _regP &= 0xfd; else _regP |= 0x02;
  if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
}

void Cpu_6502::ins_DEC(int addr, int val) {
  --val;
  val = val & 0xff;
  writeRam(addr, val);
  if (val) _regP &= 0xfd; else _regP |= 0x02;
  if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
}

void Cpu_6502::ins_EOR(int val) {
  _regA ^= val;
  if (_regA) _regP &= 0xfd; else _regP |= 0x02;
  if (_regA & 0x80) _regP |= 0x80; else _regP &= 0x7f;
}

void Cpu_6502::ins_INC(int addr, int val) {
  ++val;
  val = val & 0xff;
  writeRam(addr, val);
  if (val) _regP &= 0xfd; else _regP |= 0x02;
  if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
}

void Cpu_6502::ins_LD_(int _reg) {
  if (_reg) _regP &= 0xfd; else _regP |= 0x02;
  if (_reg & 0x80) _regP |= 0x80; else _regP &= 0x7f;
}

void Cpu_6502::ins_ORA(int val) {
  _regA |= val;
  if (_regA) _regP &= 0xfd; else _regP &= 0x02;
  if (_regA & 0x80) _regP |= 0x80; else _regP &= 0x7f;
}

void Cpu_6502::ins__reg(int _reg) {
  if (_reg) _regP &= 0xfd; else _regP |= 0x02;
  if (_reg & 0x80) _regP |= 0x80; else _regP &= 0x7f;
}

// TODO: refactor
void Cpu_6502::ins_SBC(int value ) {
  int vflag,tmp,w;
  if( (_regA ^ value ) & 0x80 )
    vflag = 1;
  else
    vflag = 0;

  if( _regP & 8 ) {
    tmp = 0xf + (_regA & 0xf) - (value & 0xf) + (_regP&1);
    if( tmp < 0x10 ) {
      w = 0;
      tmp -= 6;
    } else {
      w = 0x10;
      tmp -= 0x10;
    }
    w += 0xf0 + (_regA & 0xf0) - (value & 0xf0);
    if( w < 0x100 ) {
      _regP &= 0xfe;
      if( (_regP&0xbf) && w<0x80) _regP&=0xbf;
      w -= 0x60;
    } else {
      _regP |= 1;
      if( (_regP&0xbf) && w>=0x180) _regP&=0xbf;
    }
    w += tmp;
  } else {
    w = 0xff + _regA - value + (_regP&1);
    if( w<0x100 ) {
      _regP &= 0xfe;
      if( (_regP&0xbf) && w<0x80 ) _regP&=0xbf;
    } else {
      _regP |= 1;
      if( (_regP&0xbf) && w>= 0x180) _regP&=0xbf;
    }
  }
  _regA = w & 0xff;
  if( _regA ) _regP &= 0xfd; else _regP |= 0x02;
  if( _regA & 0x80 ) _regP |= 0x80; else _regP &= 0x7f;
}

byte Cpu_6502::fetch() {
    byte val = readRam(_regPC);
    _regPC += 1;
  return(val);
  
}

void Cpu_6502::reset(unsigned int pc) {
  _regA = _regX = _regY = 0;
  _regS = 0x100;
  _regP = 0x20;
  _regPC = pc;
}

void Cpu_6502::execIns() {
  unsigned int addr, val, zp, tmp;
  byte opcode = fetch();
 
  switch(opcode) {
    /*** ADC ***/
    case 0x61:    _dbg("ADC INDX");
      zp = (fetch() + _regX) & 0xff;
      addr = readRam(zp) + (readRam(zp) << 8);
      ins_ADC(readRam(addr));
      break;
    case 0x65:    _dbg("ADC ZP");
      ins_ADC(readRam(fetch()));
      break;
    case 0x69:    _dbg("ADC IMM");
      ins_ADC(fetch());
      break;
    case 0x6d:    _dbg("ADC ABS");
      addr = fetch() | fetch() << 8;
      ins_ADC(readRam(addr));
      break;
    case 0x71:    _dbg("ADC INDY");
      zp = fetch();
      addr = readRam(zp) + (readRam(zp + 1) << 8);
      ins_ADC(readRam(addr + _regY));
      break;
    case 0x75:    _dbg("ADC ZPX");
      addr = (fetch() + _regX) & 0xff;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | (val & 1);
      ins_ADC(val);
      break;
    case 0x79:    _dbg("ADC ABSY");
      addr = (fetch() | fetch() << 8) + _regY;
      ins_ADC(readRam(addr));
      break;
    case 0x7d:    _dbg("ADC ABSX");
      addr = (fetch() | fetch() << 8) + _regX;
      ins_ADC(readRam(addr));
      break;
    /*** ADC END ***/

    /*** AND ***/
    case 0x21:    _dbg("AND INDX");
      addr = (fetch() + _regX) & 0xff;
      ins_AND(readRam(addr) | readRam(addr + 1) << 8);
      break;
    case 0x25:    _dbg("AND ZP");
      ins_AND(readRam(fetch()));
      break;
    case 0x29:    _dbg("AND IMM");
      ins_AND(fetch());
      break;
    case 0x2d:    _dbg("AND ABS");
      addr = fetch() | fetch() << 8;
      ins_AND(readRam(addr));
      break;
    case 0x31:    _dbg("AND INDY");
      zp = fetch();
      addr = readRam(zp) + (readRam(zp + 1) << 8) + _regY;
      ins_ADC(readRam(addr));
      break;
    case 0x35:    _dbg("AND ZPX");
      addr = (fetch() + _regX) & 0xff;
      ins_AND(readRam(addr));
      break;
    case 0x39:    _dbg("AND ABXY");
      addr = (fetch() | fetch() << 8) + _regY;
      ins_AND(readRam(addr));
      break;
    case 0x3d:    _dbg("AND ABSX");
      addr = (fetch() | fetch() << 8) + _regX;
      ins_AND(readRam(addr));
      break;
    /*** AND END ***/

    /*** ASL ***/
    case 0x06:    _dbg("ASL ZP");
      val = readRam(fetch());
      _regP = (_regP & 0xfe) | ((val >> 7)&1);
      val = val << 1;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x0a:    _dbg("ASL IMPL");
      _regP = (_regP & 0xfe) | ((_regA >> 7) & 1);
      _regA = _regA << 1;
      if (_regA) _regP &= 0xfd; else _regP |= 0x02;
      if (_regA & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x16:    _dbg("ASL ZPX");
      addr = (fetch() + _regX) & 0xff;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | ((val >> 7) & 1);
      val = val << 1;
      writeRam(addr, val);
      if( val ) _regP &= 0xfd; else _regP |= 0x02;
      if( val & 0x80 ) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x0e:    _dbg("ASL ABS");
      addr = fetch() | fetch() << 8;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | ((val >> 7) & 1);
      val = val << 1;
      writeRam(addr, val);
      if( val ) _regP &= 0xfd; else _regP |= 2;
      if( val & 0x80 ) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x1e:    _dbg("ASL ABSX");
      addr = (fetch() | fetch() << 8) + _regX;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | ((val >> 7) & 1);
      val = val << 1;
      writeRam(addr, val);
      if( val ) _regP &= 0xfd; else _regP |= 0x02;
      if( val & 0x80 ) _regP |= 0x80; else _regP &= 0x7f;
      break;
    /*** ASL END ***/
    
    /*** BIT ***/
    case 0x24:    _dbg("BIT ZP");
      ins_BIT(readRam(fetch()));
      break;
    case 0x2c:    _dbg("BIT ABS");
      ins_BIT(readRam(fetch() | fetch() << 8));
      break;
    /*** BIT END ***/
    
    /*** BRANCH ***/
    case 0x10:    _dbg("BRANCH BPL");
      addr = fetch();
      if (!(_regP & 0x80)) jump_branch(addr);
      break;
    case 0x30:    _dbg("BRANCH BMI");
      addr = fetch();
      if (_regP & 0x80) jump_branch(addr);
      break;
    case 0x50:    _dbg("BRANCH BVC");
      addr = fetch();
      if (!(_regP & 0x40)) jump_branch(addr);
      break;
    case 0x70:    _dbg("BRANCH BVS");
      addr = fetch();
      if (_regP & 0x40) jump_branch(addr);
      break;
    case 0x90:    _dbg("BRANCH BCC");
      addr = fetch();
      if (!(_regP & 0x01)) jump_branch(addr);
      break;
    case 0xb0:    _dbg("BRANCH BCS");
      addr = fetch();
      if (_regP & 0x01) jump_branch(addr);
      break;
    case 0xd0:    _dbg("BRANCH BNE");
      addr = fetch();
      if (!(_regP & 0x02)) jump_branch(addr);
      break;
    case 0xf0:    _dbg("BRANCH BEQ");
      addr = fetch();
      if (_regP & 0x02) jump_branch(addr);
      break;
    /*** BRANCH END ***/
    
    /*** BRK ***/
    case 0x00:    _dbg("BRK");
      break;
    /*** BRK END ***/
    
    /*** CMP ***/
    case 0xc1:    _dbg("CMP INDX");
      zp = fetch();
      addr = readRam(zp) + (readRam(zp+1) << 8) + _regX;
      ins_CMP(_regA, readRam(addr));
      break;
    case 0xc5:    _dbg("CMP ZP");
      ins_CMP(_regA, readRam(fetch()));
      break;
    case 0xc9:    _dbg("CMP IMM");
      ins_CMP(_regA, fetch());
      break;
    case 0xcd:    _dbg("CMP ABS");
      ins_CMP(_regA, readRam(fetch() | fetch() << 8));
      break;
    case 0xd1:    _dbg("CMP INDY");
      zp = fetch();
      addr = readRam(zp) + (readRam(zp+1) << 8) + _regY;
      ins_CMP(_regA, readRam(addr));
      break;
    case 0xd5:    _dbg("CMP ZPX");
      ins_CMP(_regA, readRam(fetch() + _regX));
      break;
    case 0xd9:    _dbg("CMP ABSY");
      ins_CMP(_regA, readRam(fetch() + fetch() << 8 + _regY));
      break;
    case 0xdd:    _dbg("CMP ABSX");
      ins_CMP(_regA, readRam(fetch() + fetch() << 8 + _regX));
      break;
    /*** CMP END ***/
    
    /*** CPX ***/
    case 0xe0:    _dbg("CPX IMM");
      ins_CMP(_regX, fetch());
      break;
    case 0xe4:    _dbg("CPX ZP");
      ins_CMP(_regX, readRam(fetch()));
      break;
    case 0xec:    _dbg("CPX ABS");
      ins_CMP(_regX, readRam(fetch() | fetch() << 8));
      break;
    /*** CPX END ***/
    
    /*** CPY ***/
    case 0xc0:    _dbg("CPY IMM");
      ins_CMP(_regY, fetch());
      break;
    case 0xc4:    _dbg("CPY ZP");
      ins_CMP(_regY, readRam(fetch()));
      break;
    case 0xcc:    _dbg("CPY ABS");
      ins_CMP(_regY, readRam(fetch() | fetch() << 8));
      break;
    /*** CPY END ***/
    
    /*** DEC ***/
    case 0xc6:    _dbg("DEC ZP");
      addr = fetch();
      ins_DEC(addr, readRam(addr));
      break;
    case 0xce:    _dbg("DEC ABS");
      addr = fetch() | fetch() << 8;
      ins_DEC(addr, readRam(addr));
      break;
    case 0xd6:    _dbg("DEC ZPX");
      addr = fetch() + _regX;
      ins_DEC(addr, readRam(addr));
      break;
    case 0xde:    _dbg("DEC ABSX");
      addr = fetch() | fetch() << 8 + _regX;
      ins_DEC(addr, readRam(addr));
      break;
    /*** DEC END ***/
    
    /*** EOR ***/
    case 0x41:    _dbg("EOR INDX");
      zp = (fetch() + _regX) & 0xff;
      addr = readRam(zp) + readRam(zp+1) << 8;
      ins_EOR(readRam(addr));
      break;
    case 0x45:    _dbg("EOR ZP");
      ins_EOR(readRam(fetch()));
      break;
    case 0x49:    _dbg("EOR IMM");
      ins_EOR(fetch());
      break;
    case 0x4d:    _dbg("EOR ABS");
      ins_EOR(readRam(fetch() | fetch() << 8));
      break;
    case 0x51:    _dbg("EOR INDY");
      zp = fetch();
      addr = readRam(zp) + readRam(zp+1) << 8 + _regY;
      ins_EOR(readRam(addr));
      break;
    case 0x55:    _dbg("EOR ZPX");
      addr = (fetch() + _regX) & 0xff;
      ins_EOR(readRam(addr));
      break;
    case 0x59:    _dbg("EOR ABSY");
      ins_EOR(readRam(fetch() + fetch() << 8 + _regY));
      break;
    case 0x5d:    _dbg("EOR ABSX");
      ins_EOR(readRam(fetch() + fetch() << 8 + _regX));
      break;
    /*** EOR END ***/
    
    /*** FLAG ***/
    case 0x18:    _dbg("FLAG CLC");
      _regP &= 0xfe;
      break;
    case 0x38:    _dbg("FLAG SEC");
      _regP |= 1;
      break;
    case 0x58:    _dbg("FLAG CLI");
      break;
    case 0x78:    _dbg("FLAG SEI");
      break;
    case 0xb8:    _dbg("FLAG CLV");
      _regP &= 0xbf;
      break;
    case 0xd8:    _dbg("FLAG CLD");
      _regP &= 0xf7;
      break;
    case 0xf8:    _dbg("FLAG SED");
      _regP |= 8;
      break;
    /*** FLAG END ***/
    
    /*** INC ***/
    case 0xe6:    _dbg("DEC ZP");
      addr = fetch();
      ins_INC(addr, readRam(addr));
      break;
    case 0xee:    _dbg("INC ABS");
      addr = fetch() | fetch() << 8;
      ins_INC(addr, readRam(addr));
      break;
    case 0xf6:    _dbg("INC ZPX");
      addr = fetch() + _regX;
      ins_INC(addr, readRam(addr));
      break;
    case 0xfe:    _dbg("INC ABSX");
      addr = fetch() | fetch() << 8 + _regX;
      ins_INC(addr, readRam(addr));
      break;
    /*** INC END ***/
    
    /*** JMP ***/
    case 0x4c:    _dbg("JMP ABS");
      _regPC = fetch() | fetch() << 8;
      break;
    case 0x6c:    _dbg("JMP IND");
      break;
    /*** JMP END ***/
    
    /*** JSR ***/
    case 0x20:    _dbg("JSR ABS");
      addr = _regPC - 1;
      stack_push(addr >> 8);
      stack_push(addr & 0xff);
      _regPC = fetch() | fetch() << 8;
      break;
    /*** JSR END ***/
    
    /*** LDA ***/
    case 0xa1:    _dbg("LDA INDX");
      zp = (fetch() + _regX) & 0xff;
      addr = readRam(zp) + readRam(zp+1) << 8;
      _regA = readRam(addr);
      ins_LD_(_regA);
      break;
    case 0xa5:    _dbg("LDA ZP");
      
      addr = fetch();
      _regA = readRam(addr);
      // DEBUG
      if (addr == 0xff)
      if (Serial.available()) _regA = Serial.read() & 0xff;
      else _regA = 0;
      
      
      ins_LD_(_regA);
      break;
    case 0xa9:    _dbg("LDA IMM");
      _regA = fetch();
      ins_LD_(_regA);
      break;
    case 0xad:    _dbg("LDA ABS");
      _regA = readRam(fetch() | fetch() << 8);
      ins_LD_(_regA);
      break;
    case 0xb1:    _dbg("LDA INDY");
      zp = fetch();
      addr = readRam(zp) + (readRam(zp+1) << 8) + _regY;
      _regA = readRam(addr);
      ins_LD_(_regA);
      break;
    case 0xb5:    _dbg("LDA ZPX");
      _regA = readRam((fetch() + _regX) & 0xff);
      ins_LD_(_regA);
      break;
    case 0xb9:    _dbg("LDA ABSY");
      _regA = readRam((fetch() | fetch() << 8) + _regY);
      ins_LD_(_regA);
      break;
    case 0xbd:    _dbg("LDA ABSX");
      _regA = readRam((fetch() | fetch() << 8) + _regX);
      ins_LD_(_regA);
      break;
    /*** LDA END ***/
    
    /*** LDX ***/
    case 0xa2:    _dbg("LDX IMM");
      _regX = fetch();
      ins_LD_(_regX);
      break;
    case 0xa6:    _dbg("LDX ZP");
      _regX = readRam(fetch());
      ins_LD_(_regX);
      break;
    case 0xb6:    _dbg("LDX ZPY");
      _regX = readRam(fetch() + _regY);
      ins_LD_(_regX);
      break;
    case 0xae:    _dbg("LDX ABS");
      _regX = readRam(fetch() | fetch() << 8);
      ins_LD_(_regX);
      break;
    case 0xbe:    _dbg("LDX ABSY");
      _regX = readRam((fetch() | fetch() << 8) + _regY);
      ins_LD_(_regX);
      break;
    /*** LDX END **/

    /*** LSR ***/
    case 0x46:    _dbg("LSR ZP");
      val = readRam(fetch());
      _regP = (_regP & 0xfe) | (val & 1);
      val = val >> 1;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if ((val & 0x80) == 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x4a:    _dbg("LSR IMPL");
      _regP = (_regP & 0xfe) | (_regA & 1);
      _regA = _regA >> 1;
      if (_regA) _regP &= 0xfd; else _regP |= 0x02;
      if (_regA & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x56:    _dbg("LSR ZPX");
      addr = (fetch() + _regX) & 0xff;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | (val & 1);
      val = val >> 1;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x4e:    _dbg("LSR ABS");
      addr = fetch() | fetch() << 8;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | (val & 1);
      val = val >> 1;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 2;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x5e:    _dbg("LSR ABSX");
      addr = (fetch() | fetch() << 8) + _regX;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | (val & 1);
      val = val >> 1;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    /*** LSR END ***/
    
    /*** NOP ***/
    case 0xea:    _dbg("NOP");
      break;
    /*** NOP END ***/
    
    /*** ORA ***/
    case 0x01:    _dbg("ORA INDX");
      addr = fetch() + _regX;
      ins_ORA(readRam(addr) | readRam(addr + 1) << 8);
      break;
    case 0x05:    _dbg("ORA ZP");
      addr = fetch();
      ins_ORA(readRam(addr));
      break;
    case 0x09:    _dbg("ORA IMM");
      ins_ORA(fetch());
      break;
    case 0x0d:    _dbg("ORA ABS");
      addr = fetch() | fetch() << 8;
      ins_ORA(readRam(addr));
      break;
    case 0x11:    _dbg("ORA INDY");
      zp = fetch();
      addr = readRam(zp) + (readRam(zp+1) << 8) + _regY;
      ins_ORA(readRam(addr));
      break;
    case 0x15:    _dbg("ORA ZPX");
      addr = (fetch() + _regX) & 0xff;
      ins_ORA(readRam(addr));
      break;
    case 0x19:    _dbg("ORA ABSY");
      addr = (fetch() | fetch() << 8) + _regY;
      ins_ORA(readRam(addr));
      break;
    case 0x1d:    _dbg("ORA ABSX");
      addr = (fetch() | fetch() << 8) + _regX;
      ins_ORA(readRam(addr));
      break;
    /*** ORA END ***/
    
    /*** _regISTER ***/
    case 0xaa:    _dbg("_regISTER TAX");
      _regX = _regA & 0xff;
      ins__reg(_regX);
      break;
    case 0x8a:    _dbg("_regISTER TXA");
      _regX = _regS & 0xff;
      ins__reg(_regX);
      break;
    case 0xca:    _dbg("_regISTER DEX");
      _regX = (_regX - 1) & 0xff;
      ins__reg(_regX);
      break;
    case 0xe8:    _dbg("_regISTER INX");
      _regX = (_regX + 1) & 0xff;
      ins__reg(_regX);
      break;
    case 0xa8:    _dbg("_regISTER TAY");
      _regY = _regA & 0xff;
      ins__reg(_regY);
      break;
    case 0x98:    _dbg("_regISTER TYA");
      _regA = _regY & 0xff;
      ins__reg(_regA);
      break;
    case 0x88:    _dbg("_regISTER DEY");
      _regY = (_regY - 1) & 0xff;
      ins__reg(_regY);
      break;
    case 0xc8:    _dbg("_regISTER INY");
      _regY = (_regY + 1) & 0xff;
      ins__reg(_regY);
      break;
    /*** _regISTER END ***/
    
    /*** ROL ***/
    case 0x26:    _dbg("ROL ZP");
      tmp = _regP & 1;
      addr = fetch();
      val = readRam(addr);
      _regP = (_regP & 0xfe) | ((val >> 7) & 1);
      val = val << 1;
      val |= tmp;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x2a:    _dbg("ROL IMM");
      tmp = _regP & 1;
      _regP = (_regP & 0xfe) | ((_regA >> 7) & 1);
      _regA = _regA << 1;
      _regA |= tmp;
      if (_regA) _regP &= 0xfd; else _regP |= 0x02;
      if (_regA & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x2e:    _dbg("ROL ABS");
      tmp = _regP & 1;
      addr = fetch() + fetch() << 8;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | ((val >> 7) & 1);
      val = val << 1;
      val |= tmp;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x36:    _dbg("ROL ZPX");
      tmp = _regP & 1;
      addr = (fetch() + _regX) & 0xff;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | ((val >> 7) & 1);
      val = val << 1;
      val |= tmp;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x3e:    _dbg("ROL ABSX");
      tmp = _regP & 1;
      addr = fetch() + (fetch() << 8) + _regX;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | ((val >> 7) & 1);
      val = val << 1;
      val |= tmp;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    /*** ROL END ***/
    
    /*** ROR ***/
    case 0x66:    _dbg("ROR ZP");
      tmp = _regP & 1;
      addr = fetch();
      val = readRam(addr);
      _regP = (_regP & 0xfe) | (val & 1);
      val = val >> 1;
      if (tmp) val |= 0x80;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x6a:    _dbg("ROR IMM");
      tmp = _regP & 1;
      _regP = (_regP & 0xfe) | (_regA & 1);
      _regA = _regA >> 1;
      if (tmp) _regA |= 0x80;
      if (_regA) _regP &= 0xfd; else _regP |= 0x02;
      if (_regA & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x6e:    _dbg("ROR ABS");
      tmp = _regP & 1;
      addr = fetch() + fetch() << 8;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | (val & 1);
      val = val >> 1;
      if (tmp) val |= 0x80;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x76:    _dbg("ROR ZPX");
      tmp = _regP & 1;
      addr = (fetch() + _regX) & 0xff;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | (val & 1);
      val = val >> 1;
      if (tmp) val |= 0x80;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x7e:    _dbg("ROR ABSX");
      tmp = _regP & 1;
      addr = fetch() + (fetch() << 8) + _regX;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | (val & 1);
      val = val >> 1;
      if (tmp) val |= 0x80;
      writeRam(addr, val);
      if (val) _regP &= 0xfd; else _regP |= 0x02;
      if (val & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    /*** ROR END ***/
    
    /*** RTI ***/
    case 0x40:    _dbg("RTI");
      break;
    /*** RTI END ***/
    
    /*** RTS ***/
    case 0x60:    _dbg("RTS");
      _regPC = (stack_pop() + 1) | (stack_pop() << 8);
      break;
    /*** RTS END ***/
    
    /*** SBC ***/
    case 0xe1:    _dbg("SBC INDX");
      zp = (fetch() + _regX) & 0xff;
      addr = readRam(zp) + (readRam(zp+1) << 8);
      val = readRam(addr);
      ins_SBC(val);
      break;
    case 0xe5:    _dbg("SBC ZP");
      ins_SBC(readRam(fetch()));
      break;
    case 0xe9:    _dbg("SBC IMM");
      ins_SBC(fetch());
      break;
    case 0xed:    _dbg("SBC ABS");
      ins_SBC(readRam(fetch() + fetch() << 8));
      break;
    case 0xf1:    _dbg("SBC INDY");
      zp = fetch();
      addr = readRam(zp) + (readRam(zp+1) << 8);
      val = readRam(addr + _regY);
      ins_SBC(val);
      break;
    case 0xf5:    _dbg("SBC ZPX");
      addr = (fetch() + _regX) & 0xff;
      val = readRam(addr);
      _regP = (_regP & 0xfe) | (val & 1);
      ins_SBC(val);
      break;
    case 0xf9:    _dbg("SBC ABSY");
      val = readRam(fetch() + (fetch() << 8) + _regY);
      break;
    case 0xfd:    _dbg("SBC ABSX");
      ins_SBC(readRam(fetch() + (fetch() << 8) + _regX));
      break;
    /*** SBC END ***/
    
    /*** STA ***/
    case 0x81:    _dbg("STA INDX");
      zp = (fetch() + _regX) & 0xff;
      addr = readRam(zp) + (readRam(zp+1) << 8);
      writeRam(addr, _regA);
      break;
    case 0x85:    _dbg("STA ZP");
      addr = fetch();
      writeRam(addr, _regA);
      // DEBUG
      if (addr == 0xfd) Serial.write(_regA & 0xff);
      
      break;
    case 0x8d:    _dbg("STA ABS");
      writeRam(fetch() + fetch() << 8, _regA);
      break;
    case 0x91:    _dbg("STA INDY");
      zp = fetch();
      addr = readRam(zp) + (readRam(zp+1) << 8) + _regY;
      writeRam(addr, _regA);
      break;
    case 0x95:    _dbg("STA ZPX");
      writeRam(fetch() + _regX, _regA);
      break;
    case 0x99:    _dbg("STA ABSY");
      writeRam(fetch() + (fetch() << 8) + _regY, _regA);
      break;
    case 0x9d:    _dbg("STA ABSX");
      writeRam(fetch() + (fetch() << 8) + _regX, _regA);
      break;
    /*** STA END ***/

    /*** STACK ***/
    case 0x08:    _dbg("STACK PHP");
      stack_push(_regP);
      break;
    case 0x28:    _dbg("STACK PLP");
      _regP = stack_pop() | 0x20;
      break;
    case 0x48:    _dbg("STACK PHA");
      stack_push(_regA);
      break;
    case 0x68:    _dbg("STACK PLA");
      _regA = stack_pop();
      if (_regA) _regP &= 0xfd; else _regP |= 0x02;
      if (_regA & 0x80) _regP |= 0x80; else _regP &= 0x7f;
      break;
    case 0x9a:    _dbg("STACK TXS");
      _regS = _regX & 0xff;
      break;
    case 0xba:    _dbg("STACK TSX");
      _regX = _regS & 0xff;
      break;
    /*** STACK END ***/
    
    /*** STX ***/
    case 0x86:    _dbg("STX ZP");
      writeRam(fetch(), _regX);
      break;
    case 0x8e:    _dbg("STX ABS");
      writeRam(fetch() + (fetch() << 8), _regX);
      break;
    case 0x96:    _dbg("STX ZPY");
      writeRam(fetch() + _regY, _regX);
      break;
    /*** STX END ***/
    
    /*** STY ***/
    case 0x84:    _dbg("STX ZP");
      writeRam(fetch(), _regY);
      break;
    case 0x8c:    _dbg("STX ABS");
      writeRam(fetch() + (fetch() << 8), _regY);
      break;
    case 0x94:    _dbg("STX ZPX");
      writeRam(fetch() + _regX, _regY);
      break;
    /*** STY END ***/
      
    default:
    _dbg("Unknown ins!");
  }
}

//Cpu_6502::Cpu_6502(byte rom_dev, byte ram_dev) {
Cpu_6502::Cpu_6502(byte rom_dev) {
	_rom_dev = rom_dev;
	//_ram_dev = ram_dev;

	Wire.begin();
}


void Cpu_6502::writeRam(unsigned int addr, byte val) {
	//e2_write(_ram_dev, addr, val);
	if (addr < 0x8000) SRAM2.writestream1(addr);
	else SRAM2.writestream2(addr - 0x8000);
	SRAM2.RWdata(val);
}

byte Cpu_6502::readRam(unsigned int addr) {
	//return e2_read(_ram_dev, addr);
	if (addr < 0x8000) SRAM2.readstream1(addr);
	else SRAM2.readstream2(addr - 0x8000);
	return SRAM2.RWdata(0xff);
}

void Cpu_6502::writeRom(unsigned int addr, byte val) {
	e2_write(_rom_dev, addr, val);
}

byte Cpu_6502::readRom(unsigned int addr) {
	return e2_read(_rom_dev, addr);
}

void Cpu_6502::e2_write(byte dev, unsigned int addr, byte val) {
  Wire.beginTransmission(dev);
  Wire.write(addr >> 8);
  Wire.write(addr & 0xff);
  Wire.write(val);
  Wire.endTransmission();
  delay(5);
}

byte Cpu_6502::e2_read(byte dev, unsigned int addr) {
  Wire.beginTransmission(dev);
  Wire.write(addr >> 8);
  Wire.write(addr & 0xff);
  Wire.endTransmission(false);
  Wire.requestFrom((int)dev, 1);
  return Wire.read() & 0xff;
}