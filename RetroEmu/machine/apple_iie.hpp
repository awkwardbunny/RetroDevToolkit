#ifndef APPLE_IIE_H
#define APPLE_IIE_H

#include <common/ram.hpp>
#include <cpu/6502.hpp>
#include <QStringList>

class AppleIIe {
public:
    AppleIIe();
    ~AppleIIe();
    void step();
    void run();
    void print();

    MOS6502 *getCpu();

    RAM<uint16_t, uint8_t> *mem;

private:
    MOS6502 *cpu;
    uint32_t clk_khz;
    bool running;

    uint8_t read_mem(uint16_t);
    void write_mem(uint16_t, uint8_t);
};

#endif
