#ifndef __CPU_HPP
#define __CPU_HPP

class RECPU {
public:
    virtual void step() = 0;
    virtual void reset() = 0;
private:
};

#endif
