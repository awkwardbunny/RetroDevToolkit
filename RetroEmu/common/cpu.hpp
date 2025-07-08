#ifndef __CPU_HPP
#define __CPU_HPP

#include <common/ram.hpp>

template <typename I, typename D>
class RECPU {
public:
    // RECPU(RAM<I,D> *);
    virtual void step() = 0;
    virtual void reset() = 0;
private:
    RAM<I,D> *mem;
};

#endif
