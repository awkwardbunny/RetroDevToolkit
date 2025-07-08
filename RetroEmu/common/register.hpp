#ifndef __REGISTER_HPP
#define __REGISTER_HPP

#include <spdlog/spdlog.h>

template <typename D>
class Register {
public:
    Register(D resetVal = 0, D maskOn = 0, D maskOff = 0) {
        this->resetVal = resetVal;
        this->maskOn = maskOn;
        this->maskOff = maskOff;
        reset();
    }

    void reset() {
        data = resetVal;
        fix();
    }

    D &operator*() {
        return data;
    }

    void set(D data) {
        this->data = data;
    }

    D get() {
        return data;
    }

    void fix() {
        data |= maskOn;
        data &= ~maskOff;
    }

    void print() {
        spdlog::debug(std::format("{:x} {:x} {:x} {:x}", data, resetVal, maskOn, maskOff));
    }

private:
    D data, resetVal, maskOn, maskOff;
};

#endif






