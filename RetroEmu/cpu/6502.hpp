#ifndef MOS6502_H
#define MOS6502_H

#include <cstdint>
#include <functional>
#include <goodasm.h>

#define IS_CARRY()   (this->reg_FLAGS & 0x01)
#define IS_ZERO()    (this->reg_FLAGS & 0x02)
#define IS_INT_DIS() (this->reg_FLAGS & 0x04)
#define IS_DECIMAL() (this->reg_FLAGS & 0x08)
#define IS_BREAK()   (this->reg_FLAGS & 0x10)
#define IS_OVERFL()  (this->reg_FLAGS & 0x40)
#define IS_NEG()     (this->reg_FLAGS & 0x80)

#define SET_CARRY()   (this->reg_FLAGS |= 0x01)
#define SET_ZERO()    (this->reg_FLAGS |= 0x02)
#define SET_INT_DIS() (this->reg_FLAGS |= 0x04)
#define SET_DECIMAL() (this->reg_FLAGS |= 0x08)
#define SET_BREAK()   (this->reg_FLAGS |= 0x10)
#define SET_OVERFL()  (this->reg_FLAGS |= 0x40)
#define SET_NEG()     (this->reg_FLAGS |= 0x80)

#define CLR_CARRY()   (this->reg_FLAGS &= ~0x01)
#define CLR_ZERO()    (this->reg_FLAGS &= ~0x02)
#define CLR_INT_DIS() (this->reg_FLAGS &= ~0x04)
#define CLR_DECIMAL() (this->reg_FLAGS &= ~0x08)
#define CLR_BREAK()   (this->reg_FLAGS &= ~0x10)
#define CLR_OVERFL()  (this->reg_FLAGS &= ~0x40)
#define CLR_NEG()     (this->reg_FLAGS &= ~0x80)

#define GET_SP_VAL()  (0x100 + this->reg_SP)
#define SET_SP_VAL(x) (this->reg_SP = 0xFF & x)
#define GET_SP_REF()  (this->read_mem(0x100 + this->reg_SP))
#define SET_SP_REF(x) (this->write_mem(0x100 + this->reg_SP, x))

class MOS6502 {
public:
    MOS6502(
        std::function<void(uint16_t,uint8_t)>,
        std::function<uint8_t(uint16_t)>
        );
    ~MOS6502();
    void step();
    void reset();
    void print();

    void SetMemWriteCallback();
    void SetMemReadCallback();

    uint16_t reg_PC;
    uint8_t reg_SP, reg_A, reg_IX, reg_IY, reg_FLAGS;

private:
    bool init;

    void write_mem(uint16_t, uint8_t);
    uint8_t read_mem(uint16_t);
    uint16_t read_mem16(uint16_t);

    std::function<void(uint16_t,uint8_t)> cb_write;
    std::function<uint8_t(uint16_t)> cb_read;

    uint8_t instr_buf[4];
    uint8_t instr_buf_count;

    void push(uint8_t);
    uint8_t pop(void);

    uint8_t pull_PC8(QByteArray *instr = nullptr);
    uint16_t pull_PC16(QByteArray *instr = nullptr);

    GoodASM *gas;
    QByteArray instr;
};

#endif
