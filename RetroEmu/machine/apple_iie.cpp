#include <chrono>
#include <thread>
#include <format>
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <machine/apple_iie.hpp>

#define CPU_FREQ_KHZ (1023)
#define RAM_SIZE (64*1024)

AppleIIe::AppleIIe() {
    // spdlog::debug("AppleIIe::AppleIIe()");


    mem = new RAM<uint16_t, uint8_t>();
    mem->mapMem(0, 0xF800, true);
    mem->mapFil(0xF800, 0, "/home/brian/RetroDevToolkit/rom/apple2e_F8.bin");

    this->ram = (uint8_t *)calloc(1, RAM_SIZE);

    this->cpu = new MOS6502(
        std::bind(&AppleIIe::write_mem, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&AppleIIe::read_mem, this, std::placeholders::_1)
        );
    this->clk_khz = CPU_FREQ_KHZ;
    this->running = false;

    // for(int i = 0; i < 100; i++)
    //     this->ram[i] = i;

    std::ifstream file("rom.bin", std::ios::binary | std::ios::ate);
    if(!file.is_open()) {
        spdlog::error("Failed to open \"rom.bin\"");
    } else {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        file.read((char *)ram+0xF800, size);
    }
}

AppleIIe::~AppleIIe() {
    // spdlog::debug("AppleIIe::~AppleIIe()");
    free(this->ram);
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
    this->ram[addr] = data;
}

uint8_t AppleIIe::read_mem(uint16_t addr) {
    uint8_t data = this->ram[addr];
    // spdlog::debug(std::format("AppleIIe::read_mem() - Callback: {:04x} {:02x}", addr, data));
    return data;
}

void AppleIIe::print() {
    this->cpu->print();
}

MOS6502 *AppleIIe::getCpu() {
    return cpu;
}
