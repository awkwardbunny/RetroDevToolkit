#ifndef __MACHINE_HPP
#define __MACHINE_HPP

class REMACHINE {
public:
    // REMACHINE();
    virtual void step() = 0;
    virtual void reset() = 0;
private:
};

#endif
