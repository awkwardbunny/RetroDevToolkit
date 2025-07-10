#ifndef MOS6502_H
#define MOS6502_H

#include <cstdint>
#include <goodasm.h>
#include <common/cpu.hpp>

class MOS6502 : RECPU<uint16_t, uint8_t> {
public:
    MOS6502(RAM<uint16_t,uint8_t> *);
    ~MOS6502();
    void step();
    void reset();
    void print();
    Registers *getRegs();

private:
    bool init;
    RAM<uint16_t, uint8_t> *mem;
    Registers *regs;

    void push(uint8_t);
    uint8_t pop(void);

    uint8_t pullPC8();
    uint16_t pullPC16();
};

#endif
