#ifndef CPU_6502_H
#define CPU_6502_H

#include <Arduino.h>

class Cpu_6502
{
        public:
			Cpu_6502(byte rom_dev);
			//Cpu_6502(byte rom_dev, byte ram_dev);

			void execIns();
			void reset(unsigned int pc);

			void writeRam(unsigned int addr, byte val);
			byte readRam(unsigned int addr);

			void writeRom(unsigned int addr, byte val);
			byte readRom(unsigned int addr);

        private:
			unsigned int _regPC;
			byte _regS;
			byte _regP;
			byte _regA;
			byte _regX;
			byte _regY;

			byte _rom_dev;
			byte _ram_dev;

			void e2_write(byte dev, unsigned int addr, byte val);
			byte e2_read(byte dev, unsigned int addr);

			void jump_branch(int offset);
			void ins_ADC(int value);
			void stack_push(byte val);
			byte stack_pop();
			void ins_AND(int val);
			void ins_BIT(int val);
			void ins_CMP(int _reg, int val);
			void ins_DEC(int addr, int val);
			void ins_EOR(int val);
			void ins_INC(int addr, int val);
			void ins_LD_(int _reg);
			void ins_ORA(int val);
			void ins__reg(int _reg);
			void ins_SBC(int value);
			byte fetch();
};

#endif
// END OF FILE