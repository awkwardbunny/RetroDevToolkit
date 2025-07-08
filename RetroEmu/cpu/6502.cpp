#include <iostream>
#include <spdlog/spdlog.h>

enum OpType {
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
    CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP,
    JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI,
    RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA,
    ILL
};

enum AddrMode {
    IMP, ACC, IMM, ZERO, ZERX, ZERY, REL, ABS, ABSX, ABSY, IND, INDX, INDY, NONE
};

#include <cpu/6502.hpp>

MOS6502::MOS6502(
        std::function<void(uint16_t,uint8_t)> r,
        std::function<uint8_t(uint16_t)> w
    ) {
    // spdlog::debug("MOS6502::MOS6502()");
    this->cb_read = w;
    this->cb_write = r;
    this->reset();

    this->gas = new GoodASM("6502");
    gas->setListing("nasm");
    instr = QByteArray();
}

void MOS6502::reset() {
    this->reg_PC = 0; // (this->read_mem(0xFFFC) << 8) + this->read_mem(0xFFFD);
    this->reg_SP = 0;
    this->reg_A = 0;
    this->reg_IX = 0;
    this->reg_IY = 0;
    this->reg_FLAGS = 0;
    SET_INT_DIS();
    this->init = false;

    this->instr_buf_count = 0;
    for(int i = 0; i < 4; i++) {
        this->instr_buf[i] = 0;
    }
}

MOS6502::~MOS6502() {
    // spdlog::debug("MOS6502::~MOS6502()");
}

void MOS6502::push(uint8_t data) {
    SET_SP_REF(data);
    reg_SP--;
}

uint8_t MOS6502::pop() {
    reg_SP++;
    return GET_SP_REF();
}

uint8_t MOS6502::pull_PC8(QByteArray *instr) {
    uint8_t data = read_mem(reg_PC++);
    if(instr) instr->append(data);
    return data;
}

uint16_t MOS6502::pull_PC16(QByteArray *instr) {
    uint8_t data1 = read_mem(reg_PC++);
    uint8_t data2 = read_mem(reg_PC++);
    if(instr) instr->append(data1);
    if(instr) instr->append(data2);
    return (data2 << 8) + data1;
}

void MOS6502::step() {
    spdlog::debug("MOS6502::step()");

    if(!this->init) {
        this->init = true;
        this->reg_PC = (this->read_mem(0xFFFD) << 8) + this->read_mem(0xFFFC);
        return;
    }

    uint16_t pc_orig = reg_PC;

    // Read first byte
    instr.clear();
    uint8_t opcode = pull_PC8(&instr);

    OpType op = ILL;
    AddrMode am = NONE;
    uint16_t data = 0;

    // FETCH / DECODE
    switch(opcode) {
    // ADC
    case 0x69:
        op = ADC;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0x65:
        op = ADC;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x75:
        op = ADC;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0x6D:
        op = ADC;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0x7D:
        op = ADC;
        am = ABSX;
        data = pull_PC16(&instr);
        break;
    case 0x79:
        op = ADC;
        am = ABSY;
        data = pull_PC16(&instr);
        break;
    case 0x61:
        op = ADC;
        am = INDX;
        data = pull_PC8(&instr);
        break;
    case 0x71:
        op = ADC;
        am = INDY;
        data = pull_PC8(&instr);
        break;

    // SBC
    case 0xE9:
        op = SBC;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0xE5:
        op = SBC;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0xF5:
        op = SBC;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0xED:
        op = SBC;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0xFD:
        op = SBC;
        am = ABSX;
        data = pull_PC16(&instr);
        break;
    case 0xF9:
        op = SBC;
        am = ABSY;
        data = pull_PC16(&instr);
        break;
    case 0xE1:
        op = SBC;
        am = INDX;
        data = pull_PC8(&instr);
        break;
    case 0xF1:
        op = SBC;
        am = INDY;
        data = pull_PC8(&instr);
        break;

    // AND
    case 0x29:
        op = AND;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0x25:
        op = AND;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x35:
        op = AND;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0x2D:
        op = AND;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0x3D:
        op = AND;
        am = ABSX;
        data = pull_PC16(&instr);
        break;
    case 0x39:
        op = AND;
        am = ABSY;
        data = pull_PC16(&instr);
        break;
    case 0x21:
        op = AND;
        am = INDX;
        data = pull_PC8(&instr);
        break;
    case 0x31:
        op = AND;
        am = INDY;
        data = pull_PC8(&instr);
        break;

    // ORA
    case 0x09:
        op = ORA;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0x05:
        op = ORA;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x15:
        op = ORA;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0x0D:
        op = ORA;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0x1D:
        op = ORA;
        am = ABSX;
        data = pull_PC16(&instr);
        break;
    case 0x19:
        op = ORA;
        am = ABSY;
        data = pull_PC16(&instr);
        break;
    case 0x01:
        op = ORA;
        am = INDX;
        data = pull_PC8(&instr);
        break;
    case 0x11:
        op = ORA;
        am = INDY;
        data = pull_PC8(&instr);
        break;

    // EOR
    case 0x49:
        op = EOR;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0x45:
        op = EOR;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x55:
        op = EOR;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0x4D:
        op = EOR;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0x5D:
        op = EOR;
        am = ABSX;
        data = pull_PC16(&instr);
        break;
    case 0x59:
        op = EOR;
        am = ABSY;
        data = pull_PC16(&instr);
        break;
    case 0x41:
        op = EOR;
        am = INDX;
        data = pull_PC8(&instr);
        break;
    case 0x51:
        op = EOR;
        am = INDY;
        data = pull_PC8(&instr);
        break;

    // ASL
    case 0x0A:
        op = ASL;
        am = ACC;
        break;
    case 0x06:
        op = ASL;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x16:
        op = ASL;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0x0E:
        op = ASL;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0x1E:
        op = ASL;
        am = ABSX;
        data = pull_PC16(&instr);
        break;

    // LSR
    case 0x4A:
        op = LSR;
        am = ACC;
        break;
    case 0x46:
        op = LSR;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x56:
        op = LSR;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0x4E:
        op = LSR;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0x5E:
        op = LSR;
        am = ABSX;
        data = pull_PC16(&instr);
        break;

    // ROL
    case 0x2A:
        op = ROL;
        am = ACC;
        break;
    case 0x26:
        op = ROL;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x36:
        op = ROL;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0x2E:
        op = ROL;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0x3E:
        op = ROL;
        am = ABSX;
        data = pull_PC16(&instr);
        break;

    // ROR
    case 0x6A:
        op = ROR;
        am = ACC;
        break;
    case 0x66:
        op = ROR;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x76:
        op = ROR;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0x6E:
        op = ROR;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0x7E:
        op = ROR;
        am = ABSX;
        data = pull_PC16(&instr);
        break;

    // NOP
    case 0xEA:
        op = NOP;
        break;

    // BCC
    case 0x90:
        op = BCC;
        am = REL;
        data = pull_PC8(&instr);
        break;

    // BCS
    case 0xB0:
        op = BCS;
        am = REL;
        data = pull_PC8(&instr);
        break;

    // BEQ
    case 0xF0:
        op = BEQ;
        am = REL;
        data = pull_PC8(&instr);
        break;

    // BNE
    case 0xD0:
        op = BNE;
        am = REL;
        data = pull_PC8(&instr);
        break;

    // BIT
    case 0x24:
        op = BIT;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x2C:
        op = BIT;
        am = ABS;
        data = read_mem16(reg_PC++);
        reg_PC++;
        break;

    // BMI
    case 0x30:
        op = BMI;
        am = REL;
        data = pull_PC8(&instr);
        break;

    // BPL
    case 0x10:
        op = BPL;
        am = REL;
        data = pull_PC8(&instr);
        break;

    // BRK
    case 0x00:
        op = BRK;
        am = IMP;
        break;

    // BVC
    case 0x50:
        op = BVC;
        am = REL;
        data = pull_PC8(&instr);
        break;

    // BVS
    case 0x70:
        op = BVS;
        am = REL;
        data = pull_PC8(&instr);
        break;

    // CLC
    case 0x18:
        op = CLC;
        am = IMP;
        break;

    // SEC
    case 0x38:
        op = SEC;
        am = IMP;
        break;

    // CLD
    case 0xD8:
        op = CLD;
        am = IMP;
        break;

    // SED
    case 0xF8:
        op = SED;
        am = IMP;
        break;

    // CLI
    case 0x58:
        op = CLI;
        am = IMP;
        break;

    // SEI
    case 0x78:
        op = SEI;
        am = IMP;
        break;

    // CLV
    case 0xB8:
        op = CLV;
        am = IMP;
        break;

    // CMP
    case 0xC9:
        op = CMP;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0xC5:
        op = CMP;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0xD5:
        op = CMP;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0xCD:
        op = CMP;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0xDD:
        op = CMP;
        am = ABSX;
        data = pull_PC16(&instr);
        break;
    case 0xD9:
        op = CMP;
        am = ABSY;
        data = pull_PC16(&instr);
        break;
    case 0xC1:
        op = CMP;
        am = INDX;
        data = pull_PC8(&instr);
        break;
    case 0xD1:
        op = CMP;
        am = INDY;
        data = pull_PC8(&instr);
        break;

    // CPX
    case 0xE0:
        op = CPX;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0xE4:
        op = CPX;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0xEC:
        op = CPX;
        am = ABS;
        data = pull_PC16(&instr);
        break;

    // CPY
    case 0xC0:
        op = CPY;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0xC4:
        op = CPY;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0xCC:
        op = CPY;
        am = ABS;
        data = pull_PC16(&instr);
        break;

    // DEC
    case 0xC6:
        op = DEC;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0xD6:
        op = DEC;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0xCE:
        op = DEC;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0xDE:
        op = DEC;
        am = ABSX;
        data = pull_PC16(&instr);
        break;

    // INC
    case 0xE6:
        op = DEC;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0xF6:
        op = DEC;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0xEE:
        op = DEC;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0xFE:
        op = DEC;
        am = ABSX;
        data = pull_PC16(&instr);
        break;

    // DEX
    case 0xCA:
        op = DEX;
        am = IMP;
        break;

    // DEY
    case 0x88:
        op = DEY;
        am = IMP;
        break;

    // INX
    case 0xE8:
        op = INX;
        am = IMP;
        break;

    // INY
    case 0xC8:
        op = INY;
        am = IMP;
        break;

    // JMP
    case 0x4C:
        op = JMP;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0x6C:
        op = JMP;
        am = IND;
        data = pull_PC16(&instr);
        break;

    // JSR
    case 0x20:
        op = JSR;
        am = ABS;
        data = pull_PC16(&instr);
        break;

    // RTS
    case 0x60:
        op = RTS;
        am = IMP;
        break;

    // LDA
    case 0xA9:
        op = LDA;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0xA5:
        op = LDA;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0xB5:
        op = LDA;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0xAD:
        op = LDA;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0xBD:
        op = LDA;
        am = ABSX;
        data = pull_PC16(&instr);
        break;
    case 0xB9:
        op = LDA;
        am = ABSY;
        data = pull_PC16(&instr);
        break;
    case 0xA1:
        op = LDA;
        am = INDX;
        data = pull_PC8(&instr);
        break;
    case 0xB1:
        op = LDA;
        am = INDY;
        data = pull_PC8(&instr);
        break;

    // LDX
    case 0xA2:
        op = LDX;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0xA6:
        op = LDX;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0xB6:
        op = LDX;
        am = ZERY;
        data = pull_PC8(&instr);
        break;
    case 0xAE:
        op = LDX;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0xBE:
        op = LDX;
        am = ABSY;
        data = pull_PC16(&instr);
        break;

    // LDY
    case 0xA0:
        op = LDY;
        am = IMM;
        data = pull_PC8(&instr);
        break;
    case 0xA4:
        op = LDY;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0xB4:
        op = LDY;
        am = ZERY;
        data = pull_PC8(&instr);
        break;
    case 0xAC:
        op = LDY;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0xBC:
        op = LDY;
        am = ABSY;
        data = pull_PC16(&instr);
        break;

    // PHA
    case 0x48:
        op = PHA;
        am = IMP;
        break;

    // PLA
    case 0x68:
        op = PLA;
        am = IMP;
        break;

    // PHP
    case 0x08:
        op = PHP;
        am = IMP;
        break;

    // PLP
    case 0x28:
        op = PLP;
        am = IMP;
        break;

    // RTI
    case 0x40:
        op = RTI;
        am = IMP;
        break;

    // STA
    case 0x85:
        op = STA;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x95:
        op = STA;
        am = ZERX;
        data = pull_PC8(&instr);
        break;
    case 0x8D:
        op = STA;
        am = ABS;
        data = pull_PC16(&instr);
        break;
    case 0x9D:
        op = STA;
        am = ABSX;
        data = pull_PC16(&instr);
        break;
    case 0x99:
        op = STA;
        am = ABSY;
        data = pull_PC16(&instr);
        break;
    case 0x81:
        op = STA;
        am = INDX;
        data = pull_PC8(&instr);
        break;
    case 0x91:
        op = STA;
        am = INDY;
        data = pull_PC8(&instr);
        break;

    // STX
    case 0x86:
        op = STX;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x96:
        op = STX;
        am = ZERY;
        data = pull_PC8(&instr);
        break;
    case 0x8E:
        op = STX;
        am = ABS;
        data = pull_PC16(&instr);
        break;

    // STY
    case 0x84:
        op = STY;
        am = ZERO;
        data = pull_PC8(&instr);
        break;
    case 0x94:
        op = STY;
        am = ZERY;
        data = pull_PC8(&instr);
        break;
    case 0x8C:
        op = STY;
        am = ABS;
        data = pull_PC16(&instr);
        break;

    // TAX
    case 0xAA:
        op = TAX;
        am = IMP;
        break;

    // TAY
    case 0xA8:
        op = TAY;
        am = IMP;
        break;

    // TSX
    case 0xBA:
        op = TSX;
        am = IMP;
        break;

    // TXA
    case 0x8A:
        op = TXA;
        am = IMP;
        break;

    // TXS
    case 0x9A:
        op = TXS;
        am = IMP;
        break;

    // TYA
    case 0x98:
        op = TYA;
        am = IMP;
        break;

    }

    gas->load(instr);
    gas->baseaddress = pc_orig;
    gas->listadr = 0;
    gas->listbytes = 0;
    QList<GAInstruction> i = gas->instructions;
    spdlog::info(std::format("{:04x}: {} {}", pc_orig, i[0].verb.toStdString(), i[0].params.toStdString()));

    uint16_t orig_data = data;

    switch(am){
    case ACC:
        data = reg_A;
        break;
    case ZERO:
        data = read_mem(data);
        break;
    case ZERX:
        data = read_mem(data+reg_IX);
        break;
    case ZERY:
        data = read_mem(data+reg_IY);
        break;
    case REL:
        data += reg_PC;
        break;
    case ABSX:
        data += reg_IX;
        break;
    case ABSY:
        data += reg_IY;
        break;
    case IND:
        data = read_mem16(data);
        break;
    case INDX:
        data = read_mem((data + reg_IX) & 0xFF);
        break;
    case INDY:
        data = read_mem(read_mem(data) + reg_IY);
        break;

    case ABS:
    case IMM:
    case IMP:
    case NONE:
        break; // Do nothing
    }

    // EXECUTE
    uint8_t prev_A = reg_A;
    uint16_t scratch16;
    uint8_t scratch8;

    uint16_t result = 0;
    bool A7, B7, C7;
    switch(op) {
    case ADC:
        result = reg_A + data + (IS_CARRY() ? 1 : 0);
        reg_A = result;

    // TODO: Double-check carry and overflow
    set_overflow:
        A7 = (data & 0x80) ? 1 : 0;
        B7 = (prev_A & 0x80) ? 1 : 0;
        C7 = (result & 0x80) ? 1 : 0;
        if((A7 == B7) && (A7 != C7)) SET_OVERFL(); else CLR_OVERFL();
    set_flags:
        if(result & 0x0100) SET_CARRY(); else CLR_CARRY();
        if(reg_A & 0x80) SET_NEG(); else CLR_NEG();
        if(reg_A) CLR_ZERO(); else SET_ZERO();
        break;
    case AND:
        result = reg_A & data;
        reg_A = result;
        goto set_flags;

    case EOR:
        result = reg_A ^ data;
        reg_A = result;
        goto set_flags;

    case ORA:
        result = reg_A | data;
        reg_A = result;
        goto set_flags;

    case ASL:
        result = reg_A << 1;
        reg_A = result;
        goto set_flags;

    case BCC:
        if(!IS_CARRY()) reg_PC += (data-2);
        break;

    case BCS:
        if(IS_CARRY()) reg_PC += (data-2);
        break;

    case BEQ:
        if(IS_ZERO()) reg_PC += (data-2);
        break;

    case BIT:
        if((reg_A & data) == 0) SET_ZERO(); else CLR_ZERO();
        if(data & 0x80) SET_NEG(); else CLR_NEG();
        if(data & 0x40) SET_OVERFL(); else CLR_OVERFL();
        break;

    case BMI:
        if(IS_NEG()) reg_PC += (data-2);
        break;

    case BNE:
        if(!IS_ZERO()) reg_PC += (data-2);
        break;

    case BPL:
        if(!IS_NEG()) reg_PC += (data-2);
        break;

    case BRK:
        push(reg_PC);
        push(reg_FLAGS);
        this->reg_PC = (this->read_mem(0xFFFF) << 8) + this->read_mem(0xFFFE);
        SET_BREAK();
        break;

    case BVC:
        if(!IS_OVERFL()) reg_PC += (data-2);
        break;

    case BVS:
        if(IS_OVERFL()) reg_PC += (data-2);
        break;

    case CLC:
        CLR_CARRY();
        break;

    case CLD:
        CLR_DECIMAL();
        break;

    case CLI:
        CLR_INT_DIS();
        break;

    case CLV:
        CLR_OVERFL();
        break;

    case CMP:
        result = (reg_A - data);
        goto set_flags;

    case CPX:
        result = (reg_IX - data);
        goto set_flags;

    case CPY:
        result = (reg_IY - data);
        goto set_flags;

    case DEC:
        result = data - 1;

        // Write data back
        switch(am) {
        case ZERX:
        case ABSX:
            data += reg_IX;
        default:
            break;
        }
        write_mem(orig_data, result & 0xFF);
        goto set_flags;

    case DEX:
        result = --reg_IX;
        goto set_flags;

    case DEY:
        result = --reg_IY;
        goto set_flags;

    case INC:
        result = data + 1;

        // Write data back
        switch(am) {
        case ZERX:
        case ABSX:
            data += reg_IX;
        default:
            break;
        }
        write_mem(data, result & 0xFF);
        goto set_flags;

    case INX:
        result = ++reg_IX;
        goto set_flags;

    case INY:
        result = ++reg_IY;
        goto set_flags;
        break;

    case JMP:
        reg_PC = data;
        break;

    case JSR:
        scratch16 = reg_PC-1;
        push(scratch16 >> 8);
        push(scratch16 & 0xFF);
        reg_PC = data;
        break;

    case LDA:
        reg_A = data;
        result = reg_A;
        goto set_flags;

    case LDX:
        reg_IX = data;
        result = reg_IX;
        goto set_flags;

    case LDY:
        reg_IY = data;
        result = reg_IY;
        goto set_flags;

    case LSR:
        result = (reg_A >> 1) + ((reg_A & 1) << 8);
        goto set_flags;

    case NOP:
        break;

    case PHA:
        push(reg_A);
        break;

    case PHP:
        push(reg_FLAGS);
        break;

    case PLA:
        reg_A = pop();
        result = reg_A;
        goto set_flags;

    case PLP:
        reg_FLAGS = pop();
        break;

    case ROL:
        result = ((reg_A << 1) + IS_CARRY());
        goto set_flags;

    case ROR:
        result = ((reg_A >> 1) + (IS_CARRY() << 7) + ((reg_A & 1) << 8));
        goto set_flags;

    case RTI:
        reg_FLAGS = pop();
        reg_PC = pop();
        break;

    case RTS:
        scratch16 = pop() + 1;
        reg_PC = scratch16 + (pop() << 8);
        break;

    case SBC:
        result = (uint16_t)reg_A - (data + !IS_CARRY());
        reg_A = result;
        goto set_overflow; // TODO: I think the overflow bit works slightly different here
        // goto set_flags;

    case SEC:
        SET_CARRY();
        break;

    case SED:
        SET_DECIMAL();
        break;

    case SEI:
        SET_INT_DIS();
        break;

    case STA:
        write_mem(orig_data, reg_A); // TODO: All these store ops need the address, not the contents
        break;

    case STX:
        write_mem(orig_data, reg_IX);
        break;

    case STY:
        write_mem(orig_data, reg_IY);
        break;

    case TAX:
        reg_IX = reg_A;
        result = reg_A;
        goto set_flags;

    case TAY:
        reg_IY = reg_A;
        result = reg_A;
        goto set_flags;

    case TSX:
        reg_SP = reg_IX;
        result = reg_SP;
        goto set_flags;

    case TXA:
        reg_A = reg_IX;
        result = reg_A;
        goto set_flags;

    case TXS:
        reg_SP = reg_IX;
        break;

    case TYA:
        reg_A = reg_IY;
        result = reg_A;
        goto set_flags;

    default:
        spdlog::error(std::format("Unknown opcode: {:02x} @ 0x{:04x}", opcode, reg_PC-1));
        return;
    }

    // // FETCH
    // this->instr_buf[this->instr_buf_count++] = this->read_mem(this->reg_PC++);
    // if(instr_buf_count < 2) return;

    // // DECODE & EXECUTE
    // // In this order: http://www.6502.org/users/obelisk/6502/reference.html
    // int result = 0;
    // int A_before = reg_A;
    // switch(instr_buf[0]) {
    // case 0x69:
    //     result = reg_A + instr_buf[1];
    //     reg_A = result;
    //     spdlog::debug(std::format("ADC #0x{:04x}", instr_buf[1]));
    // set_flags:
    //     if(result > 0xFF) SET_CARRY();
    //     if(result == 0) SET_ZERO();
    //     // if() // TODO: Overflow
    // success:
    //     this->instr_buf_count = 0;
    //     for(int i = 0; i < 4; i++) {
    //         this->instr_buf[i] = 0;
    //     }
    //     break;
    // case 0x65:
    //     result = reg_A + read_mem(instr_buf[1]);
    //     reg_A = result;
    //     spdlog::debug(std::format("ADC @0x{:02x}", instr_buf[1]));
    //     goto set_flags;
    // case 0x75:

    // }

    // if(instr_buf_count == 3) {
    //     uint32_t buf = 0;
    //     for(int i = this->instr_buf_count; i > 0; i--) {
    //         buf >>= 8;
    //         buf += this->instr_buf[i-1] << 24;
    //     }
    //     spdlog::error(std::format("Illegal instruction: {:06x}", buf));
    //     instr_buf[0] = instr_buf[1];
    //     instr_buf[1] = instr_buf[2];
    //     instr_buf[2] = instr_buf[3];
    //     instr_buf[3] = 0;
    //     instr_buf_count = 2;
    // }

}

// TODO: Memory mapping different regions dynamically
void MOS6502::write_mem(uint16_t addr, uint8_t data) {
    spdlog::debug(std::format("MOS6502::write_mem() - {:04x} {:02x}", addr, data));
    this->cb_write(addr, data);
}

uint8_t MOS6502::read_mem(uint16_t addr) {
    uint8_t data = this->cb_read(addr);
    spdlog::debug(std::format("MOS6502::read_mem() - {:04x} {:02x}", addr, data));
    return data;
}

uint16_t MOS6502::read_mem16(uint16_t addr) {
    return read_mem(addr) + (read_mem(addr+1) << 8);
}

void MOS6502::print() {
    uint32_t buf = 0;
    for(int i = this->instr_buf_count; i > 0; i--) {
        buf >>= 8;
        buf += this->instr_buf[i-1] << 24;
    }

    spdlog::debug(
        std::format("MOS6502::print() - {}\nPC: {:04x}\nSP: {:04x} A: {:02x}\nIX: {:04x} IY: {:04x}\nFLAGS: {:02x}\nINSTR_BUF: {:06x}",
                    this->init ? "RUNNING" : "RESET",
                    this->reg_PC,
                    GET_SP_VAL(), this->reg_A,
                    this->reg_IX, this->reg_IY,
                    this->reg_FLAGS,
                    buf
                    )
        );
}
