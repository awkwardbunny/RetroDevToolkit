#ifndef __MACHINE_HPP
#define __MACHINE_HPP

#include <common/registers.hpp>

class REMachine {
public:
    // REMACHINE();
    virtual void step() = 0;
    virtual void reset() = 0;
    virtual Registers *getRegs() = 0;
private:
};

#endif
