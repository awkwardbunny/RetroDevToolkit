#include <chrono>
#include <thread>
#include <format>
#include <spdlog/spdlog.h>
#include <cstdlib>

#include <machine/apple_iie.hpp>

#define CPU_FREQ_KHZ (1023)
#define RAM_SIZE (64*1024)

AppleIIe::AppleIIe() {
    // spdlog::debug("AppleIIe::AppleIIe()");


    mem = new RAM<uint16_t, uint8_t>();
    mem->mapMem(0, 0xF800, true);
    mem->mapFil(0xF800, 0, "/home/brian/RetroDevToolkit/rom/apple2e_F8.bin");

    this->cpu = new MOS6502(mem);
    this->clk_khz = CPU_FREQ_KHZ;
    this->running = false;

}

AppleIIe::~AppleIIe() {
    // spdlog::debug("AppleIIe::~AppleIIe()");
    delete this->cpu;
}

void AppleIIe::step() {
    // spdlog::debug("AppleIIe::step()");
    this->cpu->step();
}

void AppleIIe::run() {
    // spdlog::debug("AppleIIe::run()");

    this->running = true;
    long cpu_period = 1000000/this->clk_khz;
    spdlog::info(std::format("AppleIIe::run() - CPU Running at {}kHz", 1000.0f/cpu_period));

    while(this->running) {
        this->step();

        // TODO: Instead of minimum fixed sleep, calculate diff and sleep appropo
        std::this_thread::sleep_for(std::chrono::nanoseconds(cpu_period));
    }
}

void AppleIIe::write_mem(uint16_t addr, uint8_t data) {
    // spdlog::debug(std::format("AppleIIe::write_mem() - Callback: {:04x} {:02x}", addr, data));
    mem->write(addr, data);
}

uint8_t AppleIIe::read_mem(uint16_t addr) {
    uint8_t data = (*mem)[addr];
    // spdlog::debug(std::format("AppleIIe::read_mem() - Callback: {:04x} {:02x}", addr, data));
    return data;
}

void AppleIIe::print() {
    this->cpu->print();
}

MOS6502 *AppleIIe::getCpu() {
    return cpu;
}
