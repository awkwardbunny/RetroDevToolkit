#ifndef __REGISTER_HPP
#define __REGISTER_HPP

#include <spdlog/spdlog.h>

template <typename D>
class Register {
public:
    Register(D reset = 0) {
        data = resetVal = reset;
    }

    void reset() {
        data = resetVal;
    }

    D &operator*() {
        return data;
    }

private:
    D data, resetVal;
};

#endif






