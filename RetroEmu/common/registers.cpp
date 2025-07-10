#include <spdlog/spdlog.h>

#include <common/registers.hpp>

Register::Register(uint8_t width, uint32_t resetVal, Display display,
                     uint32_t maskOn, uint32_t maskOff, const char *help)
    : width(width),
      mask((1 << width) - 1),
      resetVal(resetVal),
      maskOn(maskOn),
      maskOff(maskOff),
      disp(display),
      help(help) {
  reset();
}

void Register::reset() { set(resetVal); }

void Register::set(uint32_t val) {
  val &= mask;
  val |= maskOn;
  val &= ~maskOff;
  value = val;
}

uint32_t Register::get() { return value; }

void Register::operator=(uint32_t val) { set(val); }

uint32_t Register::operator*() { return get(); }

uint32_t Register::operator++() {
  set(get() + 1);
  return get();
}

uint32_t Register::operator--() {
  set(get() - 1);
  return get();
}

uint32_t Register::operator++(int) {
  uint32_t val = get();
  set(val + 1);
  return val;
}

uint32_t Register::operator--(int) {
  uint32_t val = get();
  set(val - 1);
  return val;
}

uint32_t *Register::ptr() {
  return &value;
}

Registers::Registers() { r = std::map<std::string, Register *>(); }

Register *Registers::operator[](std::string name) { return r[name]; }

void Registers::add(std::string name, Register *reg) {
  r.insert({name, reg});
}

void Registers::reset() {
  for(auto it = r.begin(); it != r.end(); it++) {
    it->second->reset();
  }
}

std::map<std::string,Register *> *Registers::getAll() {
  return &r;
}

void Registers::print() {
  for(auto it = r.begin(); it != r.end(); it++) {
    std::string name = it->first;
    Register *reg = it->second;

    int w = 0;
    switch(reg->disp) {
      case Register::DEC:
        spdlog::debug(std::format("{:8}: {}", name, **reg));
      case Register::HEX:
        w = reg->width/4;
        if(reg->width % 4) w++;
        spdlog::debug(std::format("{:8}: 0x{:0{}x}", name, **reg, w));
        break;
      case Register::OCT:
        w = (reg->width/3);
        if(reg->width % 3) w++;
        spdlog::debug(std::format("{:8}: 0o{:0{}o}", name, **reg, w));
        break;
      case Register::BIN:
        w = reg->width;
        spdlog::debug(std::format("{:8}: 0b{:0{}b}", name, **reg, w));
        break;
    }
  }
}