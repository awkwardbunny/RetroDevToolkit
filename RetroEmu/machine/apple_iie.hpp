#ifndef APPLE_IIE_H
#define APPLE_IIE_H

#include <common/ram.hpp>
#include <common/machine.hpp>
#include <cpu/6502.hpp>

class AppleIIe : public REMachine {
public:
    uint32_t clk_khz;

    AppleIIe();
    ~AppleIIe();

    void reset();
    void step();
    void print();

    Registers *getRegs();
    RAM<uint16_t, uint8_t> *mem;

private:
    MOS6502 *cpu;

    // uint8_t read_mem(uint16_t);
    // void write_mem(uint16_t, uint8_t);
};

#endif
