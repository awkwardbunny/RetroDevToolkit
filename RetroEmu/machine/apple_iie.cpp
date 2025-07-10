#include <chrono>
#include <thread>
#include <format>
#include <spdlog/spdlog.h>
#include <cstdlib>

#include <machine/apple_iie.hpp>

#define CPU_FREQ_KHZ (1023)

AppleIIe::AppleIIe() {
    // spdlog::debug("AppleIIe::AppleIIe()");


    mem = new RAM<uint16_t, uint8_t>();
    mem->mapMem("", 0, 0xF800, true);
    mem->mapFil("monitor", 0xF800, 0, "/home/brian/RetroDevToolkit/rom/apple2e_F8.bin");
    mem->printMap();

    this->cpu = new MOS6502(mem);
    this->clk_khz = CPU_FREQ_KHZ;

}

AppleIIe::~AppleIIe() {
    // spdlog::debug("AppleIIe::~AppleIIe()");
    delete this->cpu;
}

Registers *AppleIIe::getRegs() {
    return cpu->getRegs();
}

void AppleIIe::reset() {
    cpu->reset();
}

void AppleIIe::step() {
    // spdlog::debug("AppleIIe::step()");
    cpu->step();
}

void AppleIIe::unload() {
    mem->unmap("test");
}

void AppleIIe::load(const char *path, uint16_t addr) {
    mem->mapFil("test", addr, 0x100, path, true);
}

// void AppleIIe::write_mem(uint16_t addr, uint8_t data) {
//     // spdlog::debug(std::format("AppleIIe::write_mem() - Callback: {:04x} {:02x}", addr, data));
//     mem->write(addr, data);
// }

// uint8_t AppleIIe::read_mem(uint16_t addr) {
//     uint8_t data = (*mem)[addr];
//     // spdlog::debug(std::format("AppleIIe::read_mem() - Callback: {:04x} {:02x}", addr, data));
//     return data;
// }

void AppleIIe::print() {
    cpu->print();
}