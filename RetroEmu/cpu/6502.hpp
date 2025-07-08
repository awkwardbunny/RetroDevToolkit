#ifndef MOS6502_H
#define MOS6502_H

#include <cstdint>
#include <goodasm.h>
#include <common/cpu.hpp>
#include <common/register.hpp>

#define MEM (*mem)
#define REG_PC (**regPC)
#define REG_SP (**regSP)
#define REG_FLAGS (**regFLAGS)
#define REG_A (**regA)
#define REG_X (**regX)
#define REG_Y (**regY)

#define IS_CARRY()   (REG_FLAGS & 0x01)
#define IS_ZERO()    (REG_FLAGS & 0x02)
#define IS_INT_DIS() (REG_FLAGS & 0x04)
#define IS_DECIMAL() (REG_FLAGS & 0x08)
#define IS_BREAK()   (REG_FLAGS & 0x10)
#define IS_OVERFL()  (REG_FLAGS & 0x40)
#define IS_NEG()     (REG_FLAGS & 0x80)

#define SET_CARRY()   (REG_FLAGS |= 0x01)
#define SET_ZERO()    (REG_FLAGS |= 0x02)
#define SET_INT_DIS() (REG_FLAGS |= 0x04)
#define SET_DECIMAL() (REG_FLAGS |= 0x08)
#define SET_BREAK()   (REG_FLAGS |= 0x10)
#define SET_OVERFL()  (REG_FLAGS |= 0x40)
#define SET_NEG()     (REG_FLAGS |= 0x80)

#define CLR_CARRY()   (REG_FLAGS &= ~0x01)
#define CLR_ZERO()    (REG_FLAGS &= ~0x02)
#define CLR_INT_DIS() (REG_FLAGS &= ~0x04)
#define CLR_DECIMAL() (REG_FLAGS &= ~0x08)
#define CLR_BREAK()   (REG_FLAGS &= ~0x10)
#define CLR_OVERFL()  (REG_FLAGS &= ~0x40)
#define CLR_NEG()     (REG_FLAGS &= ~0x80)

class MOS6502 : RECPU<uint16_t, uint8_t> {
public:
    MOS6502(RAM<uint16_t,uint8_t> *);
    ~MOS6502();
    void step();
    void reset();
    void print();

private:
    RAM<uint16_t, uint8_t> *mem;

    Register<uint16_t> *regPC, *regSP;
    Register<uint8_t> *regFLAGS, *regA, *regX, *regY;

    void push(uint8_t);
    uint8_t pop(void);

    uint8_t pullPC8();
    uint16_t pullPC16();

    GoodASM *gas;
};

#endif
