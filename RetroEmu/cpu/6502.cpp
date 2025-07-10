#include <spdlog/spdlog.h>

#include <cpu/6502.hpp>

#define MEM (*mem)
#define REG_PC (*(*regs)["PC"])
#define REG_SP (*(*regs)["SP"])
#define REG_FLAGS (*(*regs)["FLAGS"])
#define REG_A (*(*regs)["A"])
#define REG_X (*(*regs)["X"])
#define REG_Y (*(*regs)["Y"])

#define IS_CARRY()   (*REG_FLAGS & 0x01)
#define IS_ZERO()    (*REG_FLAGS & 0x02)
#define IS_INT_DIS() (*REG_FLAGS & 0x04)
#define IS_DECIMAL() (*REG_FLAGS & 0x08)
#define IS_BREAK()   (*REG_FLAGS & 0x10)
#define IS_OVERFL()  (*REG_FLAGS & 0x40)
#define IS_NEG()     (*REG_FLAGS & 0x80)

#define SET_CARRY()   (REG_FLAGS = *REG_FLAGS | 0x01)
#define SET_ZERO()    (REG_FLAGS = *REG_FLAGS | 0x02)
#define SET_INT_DIS() (REG_FLAGS = *REG_FLAGS | 0x04)
#define SET_DECIMAL() (REG_FLAGS = *REG_FLAGS | 0x08)
#define SET_BREAK()   (REG_FLAGS = *REG_FLAGS | 0x10)
#define SET_OVERFL()  (REG_FLAGS = *REG_FLAGS | 0x40)
#define SET_NEG()     (REG_FLAGS = *REG_FLAGS | 0x80)

#define CLR_CARRY()   (REG_FLAGS = *REG_FLAGS & ~0x01)
#define CLR_ZERO()    (REG_FLAGS = *REG_FLAGS & ~0x02)
#define CLR_INT_DIS() (REG_FLAGS = *REG_FLAGS & ~0x04)
#define CLR_DECIMAL() (REG_FLAGS = *REG_FLAGS & ~0x08)
#define CLR_BREAK()   (REG_FLAGS = *REG_FLAGS & ~0x10)
#define CLR_OVERFL()  (REG_FLAGS = *REG_FLAGS & ~0x40)
#define CLR_NEG()     (REG_FLAGS = *REG_FLAGS & ~0x80)

#define IMM data
#define IMP data
#define ACC *REG_A
#define ZERO MEM[data]
#define ZERX MEM[(data+*REG_X) & 0xFF]
#define ZERY MEM[(data+*REG_Y) & 0xFF]
#define REL data
#define ABS ZERO
#define ABSX MEM[data+*REG_X]
#define ABSY MEM[data+*REG_Y]
#define IND MEM[ZERO]
#define INDX MEM[ZERX]
#define INDY MEM[ZERO + *REG_Y]

MOS6502::MOS6502(RAM<uint16_t, uint8_t> *mem) : mem(mem), init(false) {

    // gas = new GoodASM("6502");
    // gas->setListing("nasm");
    regs = new Registers();
    regs->add("PC", new Register(16));
    regs->add("SP", new Register(9, 0, Register::HEX, 0x0100));
    regs->add("FLAGS", new Register(8, 0x04, Register::BIN, 0, 0x20, "CZIDB-VN"));
    regs->add("A", new Register(8));
    regs->add("X", new Register(8));
    regs->add("Y", new Register(8));
}

void MOS6502::reset() {
    init = false;
    regs->reset();
}

Registers *MOS6502::getRegs() {
    return regs;
}

MOS6502::~MOS6502() {
    // spdlog::debug("MOS6502::~MOS6502()");
}

void MOS6502::print() {
    regs->print();
}

void MOS6502::push(uint8_t data) {
    mem->write(REG_SP--, data);
    // spdlog::debug(std::format("{:04x}", *REG_SP));
}

uint8_t MOS6502::pop() {
    REG_SP++;
    // spdlog::debug(std::format("{:04x}", *REG_SP));
    return MEM[*REG_SP];
}

uint8_t MOS6502::pullPC8() {
    return MEM[REG_PC++];
}

uint16_t MOS6502::pullPC16() {
    uint8_t data1 = MEM[REG_PC++];
    uint8_t data2 = MEM[REG_PC++];
    return (data2 << 8) + data1;
}

void MOS6502::step() {
    spdlog::debug("MOS6502::step()");

    if(!init) {
        spdlog::debug("Init");
        init = true;
        *((*regs)["PC"]) = ((*mem)[0xFFFD] << 8) + (*mem)[0xFFFC];

        print();
        return;
    }

    uint16_t origPC = *REG_PC;

    // Read first byte
    uint8_t opcode = pullPC8();

    uint16_t result = 0;
    uint16_t data = 0;

    bool A7, B7, C7;
    B7 = (*REG_A & 0x80);

    // FETCH / DECODE / EXECUTE
    switch(opcode) {
    // ADC
    case 0x69:
        data = pullPC8();
        data = IMM;
    adc:
        result = *REG_A + data + IS_CARRY();
        REG_A = result;
    // TODO: Double-check carry and overflow
    set_overflow:
        A7 = (data & 0x80);
        C7 = (result & 0x80);
        if((A7 == B7) && (A7 != C7)) SET_OVERFL(); else CLR_OVERFL();
    set_flags:
        if(result & 0x0100) SET_CARRY(); else CLR_CARRY();
        if(*REG_A & 0x80) SET_NEG(); else CLR_NEG();
        if(*REG_A) CLR_ZERO(); else SET_ZERO();
        break;
    case 0x65:
        data = pullPC8();
        data = ZERO;
        goto adc;
    case 0x75:
        data = pullPC8();
        data = ZERX;
        goto adc;
    case 0x6D:
        data = pullPC16();
        data = ABS;
        goto adc;
    case 0x7D:
        data = pullPC16();
        data = ABSX;
        goto adc;
    case 0x79:
        data = pullPC16();
        data = ABSY;
        goto adc;
    case 0x61:
        data = pullPC8();
        data = INDX;
        goto adc;
    case 0x71:
        data = pullPC8();
        data = INDY;
        goto adc;

    // SBC
    case 0xE9:
        data = pullPC8();
        data = IMM;
    sbc:
        result = (uint16_t)*REG_A - (data + !IS_CARRY());
        REG_A = result;
        goto set_overflow; // I think overflow works a bit differently here
    case 0xE5:
        data = pullPC8();
        data = ZERO;
        goto sbc;
    case 0xF5:
        data = pullPC8();
        data = ZERX;
        goto sbc;
    case 0xED:
        data = pullPC16();
        data = ABS;
        goto sbc;
    case 0xFD:
        data = pullPC16();
        data = ABSX;
        goto sbc;
    case 0xF9:
        data = pullPC16();
        data = ABSY;
        goto sbc;
    case 0xE1:
        data = pullPC8();
        data = INDX;
        goto sbc;
    case 0xF1:
        data = pullPC8();
        data = INDY;
        goto sbc;

    // AND
    case 0x29:
        data = pullPC8();
        data = IMM;
    and_i:
        REG_A = *REG_A & data;
        goto set_flags;
    case 0x25:
        data = pullPC8();
        data = ZERO;
        goto and_i;
    case 0x35:
        data = pullPC8();
        data = ZERX;
        goto and_i;
    case 0x2D:
        data = pullPC16();
        data = ABS;
        goto and_i;
    case 0x3D:
        data = pullPC16();
        data = ABSX;
        goto and_i;
    case 0x39:
        data = pullPC16();
        data = ABSY;
        goto and_i;
    case 0x21:
        data = pullPC8();
        data = INDX;
        goto and_i;
    case 0x31:
        data = pullPC8();
        data = INDY;
        goto and_i;

    // ORA
    case 0x09:
        data = pullPC8();
        data = IMM;
    ora:
        REG_A = *REG_A | data;
        goto set_flags;
    case 0x05:
        data = pullPC8();
        data = ZERO;
        goto ora;
    case 0x15:
        data = pullPC8();
        data = ZERX;
        goto ora;
    case 0x0D:
        data = pullPC16();
        data = ABS;
        goto ora;
    case 0x1D:
        data = pullPC16();
        data = ABSX;
        goto ora;
    case 0x19:
        data = pullPC16();
        data = ABSY;
        goto ora;
    case 0x01:
        data = pullPC8();
        data = INDX;
        goto ora;
    case 0x11:
        data = pullPC8();
        data = INDY;
        goto ora;

    // EOR
    case 0x49:
        data = pullPC8();
        data = IMM;
    eor:
        REG_A = *REG_A ^ data;
        goto set_flags;
    case 0x45:
        data = pullPC8();
        data = ZERO;
        goto eor;
    case 0x55:
        data = pullPC8();
        data = ZERX;
        goto eor;
    case 0x4D:
        data = pullPC16();
        data = ABS;
        goto eor;
    case 0x5D:
        data = pullPC16();
        data = ABSX;
        goto eor;
    case 0x59:
        data = pullPC16();
        data = ABSY;
        goto eor;
    case 0x41:
        data = pullPC8();
        data = INDX;
        goto eor;
    case 0x51:
        data = pullPC8();
        data = INDY;
        goto eor;

    // ASL
    case 0x0A:
        data = ACC;
    asl:
        result = data << 1;
    set_a:
        REG_A = result;
        goto set_flags;
    case 0x06:
        data = pullPC8();
        data = ZERO;
        goto asl;
    case 0x16:
        data = pullPC8();
        data = ZERX;
        goto asl;
    case 0x0E:
        data = pullPC16();
        data = ABS;
        goto asl;
    case 0x1E:
        data = pullPC16();
        data = ABSX;
        goto asl;

    // LSR
    case 0x4A:
        data = ACC;
    lsr:
        result = (data >> 1) + ((data & 1) << 8);
        goto set_a;
    case 0x46:
        data = pullPC8();
        data = ZERO;
        goto lsr;
    case 0x56:
        data = pullPC8();
        data = ZERX;
        goto lsr;
    case 0x4E:
        data = pullPC16();
        data = ABS;
        goto lsr;
    case 0x5E:
        data = pullPC16();
        data = ABSX;
        goto lsr;

    // ROL
    case 0x2A:
        data = ACC;
    rol:
        result = ((data << 1) + IS_CARRY());
        goto set_a;
    case 0x26:
        data = pullPC8();
        data = ZERO;
        goto rol;
    case 0x36:
        data = pullPC8();
        data = ZERX;
        goto rol;
    case 0x2E:
        data = pullPC16();
        data = ABS;
        goto rol;
    case 0x3E:
        data = pullPC16();
        data = ABSX;
        goto rol;

    // ROR
    case 0x6A:
        data = ACC;
    ror:
        result = ((data >> 1) + (IS_CARRY() << 7) + ((data & 1) << 8));
        goto set_a;
    case 0x66:
        data = pullPC8();
        data = ZERO;
        goto ror;
    case 0x76:
        data = pullPC8();
        data = ZERX;
        goto ror;
    case 0x6E:
        data = pullPC16();
        data = ABS;
        goto ror;
    case 0x7E:
        data = pullPC16();
        data = ABSX;
        goto ror;

    // NOP
    case 0xEA:
        break;

    // BCC
    case 0x90:
        data = pullPC8();
        data = REL;
    rel_branch:
        REG_PC = (int16_t)*REG_PC + (int8_t)data;
        break;

    // BCS
    case 0xB0:
        data = pullPC8();
        data = REL;
        goto rel_branch;

    // BEQ
    case 0xF0:
        data = pullPC8();
        data = REL;
        goto rel_branch;

    // BNE
    case 0xD0:
        data = pullPC8();
        data = REL;
        goto rel_branch;

    // BIT
    case 0x24:
        data = pullPC8();
        data = ZERO;
    bit:
        (*REG_A & data) ? CLR_ZERO() : SET_ZERO();
        (data & 0x80) ? SET_NEG() : CLR_NEG();
        (data & 0x40) ? SET_OVERFL() : CLR_OVERFL();
        break;
    case 0x2C:
        data = pullPC16();
        data = ABS;
        goto bit;

    // BMI
    case 0x30:
        data = pullPC8();
        data = REL;
        goto rel_branch;

    // BPL
    case 0x10:
        data = pullPC8();
        data = REL;
        goto rel_branch;

    // BRK
    case 0x00:
        data = IMP;
        push(*REG_PC >> 8);
        push(*REG_PC & 0xFF);
        push(*REG_FLAGS);
        REG_PC = (MEM[0xFFFF] << 8) + MEM[0xFFFE];
        SET_BREAK();
        break;

    // BVC
    case 0x50:
        data = pullPC8();
        data = REL;
        goto rel_branch;

    // BVS
    case 0x70:
        data = pullPC8();
        data = REL;
        goto rel_branch;

    // CLC
    case 0x18:
        data = IMP;
        CLR_CARRY();
        break;

    // SEC
    case 0x38:
        data = IMP;
        SET_CARRY();
        break;

    // CLD
    case 0xD8:
        CLR_DECIMAL();
        data = IMP;
        break;

    // SED
    case 0xF8:
        data = IMP;
        SET_DECIMAL();
        break;

    // CLI
    case 0x58:
        data = IMP;
        CLR_INT_DIS();
        break;

    // SEI
    case 0x78:
        data = IMP;
        SET_INT_DIS();
        break;

    // CLV
    case 0xB8:
        data = IMP;
        CLR_OVERFL();
        break;

    // CMP
    case 0xC9:
        data = pullPC8();
        data = IMM;
    cmp:
        result = *REG_A - data;
    set_result_flags:
        if(result & 0x80) SET_NEG(); else CLR_NEG();
        if(result & 0xFF) CLR_ZERO(); else SET_ZERO();
        break;
    case 0xC5:
        data = pullPC8();
        data = ZERO;
        goto cmp;
    case 0xD5:
        data = pullPC8();
        data = ZERX;
        goto cmp;
    case 0xCD:
        data = pullPC16();
        data = ABS;
        goto cmp;
    case 0xDD:
        data = pullPC16();
        data = ABSX;
        goto cmp;
    case 0xD9:
        data = pullPC16();
        data = ABSY;
        goto cmp;
    case 0xC1:
        data = pullPC8();
        data = INDX;
        goto cmp;
    case 0xD1:
        data = pullPC8();
        data = INDY;
        goto cmp;

    // CPX
    case 0xE0:
        data = pullPC8();
        data = IMM;
    cpx:
        result = *REG_X - data;
        goto set_result_flags;
    case 0xE4:
        data = pullPC8();
        data = ZERO;
        goto cpx;
    case 0xEC:
        data = pullPC16();
        data = ABS;
        goto cpx;

    // CPY
    case 0xC0:
        data = pullPC8();
        data = IMM;
    cpy:
        result = *REG_Y - data;
        goto set_result_flags;
    case 0xC4:
        data = pullPC8();
        data = ZERO;
        goto cpy;
    case 0xCC:
        data = pullPC16();
        data = ABS;
        goto cpy;

    // DEC
    case 0xC6:
        data = pullPC8();
        result = ZERO;
        mem->write(data, result-1);
        break;
    case 0xD6:
        data = pullPC8();
        result = ZERX;
        mem->write((data+*REG_X) & 0xFF, result-1);
        break;
    case 0xCE:
        data = pullPC16();
        result = ABS;
        mem->write(data, result-1);
        break;
    case 0xDE:
        data = pullPC16();
        data = ABSX;
        mem->write(data+*REG_X, result-1);
        break;

    // INC
    case 0xE6:
        data = pullPC8();
        data = ZERO;
        mem->write(data, result+1);
        break;
    case 0xF6:
        data = pullPC8();
        data = ZERX;
        mem->write((data+*REG_X) & 0xFF, result+1);
        break;
    case 0xEE:
        data = pullPC16();
        data = ABS;
        mem->write(data, result+1);
        break;
    case 0xFE:
        data = pullPC16();
        data = ABSX;
        mem->write(data+*REG_X, result+1);
        break;

    // DEX
    case 0xCA:
        data = IMP;
        REG_X--;
    set_x_flags:
        if(*REG_X & 0x80) SET_NEG(); else CLR_NEG();
        if(*REG_X & 0xFF) CLR_ZERO(); else SET_ZERO();
        break;

    // DEY
    case 0x88:
        data = IMP;
        REG_Y--;
    set_y_flags:
        if(*REG_Y & 0x80) SET_NEG(); else CLR_NEG();
        if(*REG_Y & 0xFF) CLR_ZERO(); else SET_ZERO();
        break;

    // INX
    case 0xE8:
        data = IMP;
        REG_X++;
        goto set_x_flags;

    // INY
    case 0xC8:
        data = IMP;
        REG_Y++;
        goto set_y_flags;

    // JMP
    case 0x4C:
        data = pullPC16();
        // data = ABS;
    jmp:
        REG_PC = data;
        break;
    case 0x6C:
        data = pullPC16();
        data = IND;
        goto jmp;

    // JSR
    case 0x20:
        data = pullPC16();
        // data = ABS;
        push((*REG_PC-1) >> 8);
        push((*REG_PC-1) & 0xFF);
        goto jmp;

    // RTS
    case 0x60:
        data = IMP;
        data = pop();
        data += (pop() << 8) + 1;
        goto jmp;

    // LDA
    case 0xA9:
        data = pullPC8();
        data = IMM;
    lda:
        REG_A = data;
        goto set_flags;
    case 0xA5:
        data = pullPC8();
        data = ZERO;
        goto lda;
    case 0xB5:
        data = pullPC8();
        data = ZERX;
        goto lda;
    case 0xAD:
        data = pullPC16();
        data = ABS;
        goto lda;
    case 0xBD:
        data = pullPC16();
        data = ABSX;
        goto lda;
    case 0xB9:
        data = pullPC16();
        data = ABSY;
        goto lda;
    case 0xA1:
        data = pullPC8();
        data = INDX;
        goto lda;
    case 0xB1:
        data = pullPC8();
        data = INDY;
        goto lda;

    // LDX
    case 0xA2:
        data = pullPC8();
        data = IMM;
    ldx:
        REG_X = data;
        goto set_x_flags;
    case 0xA6:
        data = pullPC8();
        data = ZERO;
        goto ldx;
    case 0xB6:
        data = pullPC8();
        data = ZERY;
        goto ldx;
    case 0xAE:
        data = pullPC16();
        data = ABS;
        goto ldx;
    case 0xBE:
        data = pullPC16();
        data = ABSY;
        goto ldx;

    // LDY
    case 0xA0:
        data = pullPC8();
        data = IMM;
    ldy:
        REG_Y = data;
        goto set_y_flags;
    case 0xA4:
        data = pullPC8();
        data = ZERO;
        goto ldy;
    case 0xB4:
        data = pullPC8();
        data = ZERY;
        goto ldy;
    case 0xAC:
        data = pullPC16();
        data = ABS;
        goto ldy;
    case 0xBC:
        data = pullPC16();
        data = ABSY;
        goto ldy;

    // PHA
    case 0x48:
        data = IMP;
        push(*REG_A);
        break;

    // PLA
    case 0x68:
        data = IMP;
        REG_A = pop();
        goto set_flags;

    // PHP
    case 0x08:
        data = IMP;
        push(*REG_FLAGS);
        break;

    // PLP
    case 0x28:
        data = IMP;
        REG_FLAGS = pop();
        break;

    // RTI
    case 0x40:
        data = IMP;
        REG_FLAGS = pop();
        REG_PC = pop();
        REG_PC = pop() << 8;
        break;

    // STA
    case 0x85:
        data = pullPC8();
        // data = ZERO;
        mem->write(data, *REG_A);
        break;
    case 0x95:
        data = pullPC8();
        // data = ZERX;
        mem->write((data+*REG_X) & 0xFF, *REG_A);
        break;
    case 0x8D:
        data = pullPC16();
        // data = ABS;
        mem->write(data & 0xFF, *REG_A);
        break;
    case 0x9D:
        data = pullPC16();
        // data = ABSX;
        mem->write(data+*REG_X & 0xFF, *REG_A);
        break;
    case 0x99:
        data = pullPC16();
        // data = ABSY;
        mem->write((data+*REG_Y) & 0xFF, *REG_A);
        break;
    case 0x81:
        data = pullPC8();
        // data = INDX;
        mem->write(ZERX, *REG_A);
        break;
    case 0x91:
        data = pullPC8();
        // data = INDY;
        mem->write(ZERO+*REG_Y, *REG_A);
        break;

    // STX
    case 0x86:
        data = pullPC8();
        // data = ZERO;
        mem->write(data, *REG_X);
        break;
    case 0x96:
        data = pullPC8();
        // data = ZERY;
        mem->write((data+*REG_Y) & 0xFF, *REG_X);
        break;
    case 0x8E:
        data = pullPC16();
        // data = ABS;
        mem->write(data, *REG_X);
        break;

    // STY
    case 0x84:
        data = pullPC8();
        // data = ZERO;
        mem->write(data, *REG_Y);
        break;
    case 0x94:
        data = pullPC8();
        // data = ZERY;
        mem->write((data+*REG_Y) & 0xFF, *REG_Y);
        break;
    case 0x8C:
        data = pullPC16();
        // data = ABS;
        mem->write(data, *REG_Y);
        break;

    // TAX
    case 0xAA:
        data = IMP;
        REG_X = *REG_A;
        goto set_x_flags;

    // TAY
    case 0xA8:
        data = IMP;
        REG_Y = *REG_A;
        goto set_y_flags;

    // TSX
    case 0xBA:
        data = IMP;
        REG_X = *REG_SP;
        goto set_x_flags;

    // TXA
    case 0x8A:
        data = IMP;
        REG_A = *REG_X;
        goto set_flags;

    // TXS
    case 0x9A:
        data = IMP;
        REG_SP = *REG_X;
        break;

    // TYA
    case 0x98:
        data = IMP;
        REG_A = *REG_Y;
        goto set_flags;

    default:
        spdlog::error(std::format("Unknown opcode: {:02x} @ 0x{:04x}", opcode, origPC));
        break;
    }
}
