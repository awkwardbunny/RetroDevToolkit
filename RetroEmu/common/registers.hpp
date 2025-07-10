#ifndef __REGINFO_HPP
#define __REGINFO_HPP

#include <common/ram.hpp>
#include <map>

class Register {
public:
    enum Display { DEC, HEX, OCT, BIN };

    uint8_t  width;
    Display disp = HEX;
    const char *help;

    Register(
        uint8_t width = 0,
        uint32_t resetVal = 0,
        Display display = HEX,
        uint32_t maskOn = 0,
        uint32_t maskOff = 0,
        const char *help = nullptr
    );
    
    void reset();
    void set(uint32_t);
    uint32_t get();

    void operator=(uint32_t);
    uint32_t operator*();
    uint32_t operator++();
    uint32_t operator--();
    uint32_t operator++(int);
    uint32_t operator--(int);

    uint32_t *ptr();

private:
    uint32_t value;
    uint32_t mask;
    uint32_t resetVal = 0;
    uint32_t maskOn = 0;
    uint32_t maskOff = 0;
};

class Registers {
public:
    Registers();
    // ~Registers();

    Register *operator[](std::string);
    void add(std::string, Register *);
    void print();
    void reset();
    std::map<std::string,Register *> *getAll();

private:
    std::map<std::string,Register *> r;
};

#endif
