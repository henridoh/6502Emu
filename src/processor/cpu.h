#include "datatypes.h"
using namespace data_types;
#include "memory.h"
#include <vector>
#include <functional>


#define N 0b10000000 // negative
#define V 0b01000000 // overflow
#define B 0b00010000 // break
#define D 0b00001000 // decimal
#define I 0b00000100 // irq disable
#define Z 0b00000010 // zero
#define C 0b00000001 // carry

class CPU{
  private:
    struct{
      byte A;
      byte X;
      byte Y;
      word PC;
      byte S;
      byte P;
    } reg = {
      .A=0,
      .X=0,
      .Y=0,
      .PC=0x600,
      .S=0xFF,
      .P=0,
    };

    struct{
      bool IRQ;
      bool NMI;
      word IRQ_VECTOR;
      word NMI_VECTOR;
    } interrupts = {
      .IRQ = false,
      .NMI = false,
      .IRQ_VECTOR = 0xFFFE,
      .NMI_VECTOR = 0xFFFA,
    };

    void push(byte value);
    byte pop();

    // flag operations
    bool is_negative_set();
    bool is_overflow_set();
    bool is_break_set();
    bool is_decimal_set();
    bool is_irq_disable_set();
    bool is_zero_set();
    bool is_carry_set();

    void set_negative();
    void set_overflow();
    void set_break();
    void set_decimal();
    void set_irq_disable();
    void set_zero();
    void set_carry();

    void set_negative(word state);
    void set_overflow(word state);
    void set_break(word state);
    void set_decimal(word state);
    void set_irq_disable(word state);
    void set_zero(word state);
    void set_carry(word state);

    void clear_negative();
    void clear_overflow();
    void clear_break();
    void clear_decimal();
    void clear_irq_disable();
    void clear_zero();
    void clear_carry();

    // memory operations
    byte fetch_byte();
    word fetch_word();

    // addressing modes
    word get_a();
    word get_abs();
    word get_abs_x();
    word get_abs_y();
    word get_imm();
    word get_imp();
    word get_ind();
    word get_x_ind();
    word get_ind_y();
    word get_rel();
    word get_zpg();
    word get_zpg_x();
    word get_zpg_y();
    word get_noa();

    byte add_co(byte a, byte b);

    // instruction set
    enum instructions {
    BRK, ORA, NOP, ASL, PHP, BPL, CLC, JSR, AND, BIT,
    ROL, PLP, BMI, SEC, RTI, EOR, LSR, PHA, JMP, BVC,
    CLI, RTS, ADC, ROR, PLA, SEI, BVS, STA, STY, STX,
    DEY, TXA, BCC, TYA, TXS, LDY, LDA, LDX, TAY, TAX,
    BCS, CLV, TSX, CPY, CMP, DEC, INY, DEX, BNE,
    CLD, CPX, INC, INX, SBC, BEQ, SED,
    };

    enum address_modes {
      acc, abs, abx, aby, imm, imp, ind,
      xin, iny, rel, zpg, zpx, zpy, NOA
    };

    byte instruction_table[0x100] = {
    /*        X0   X1   X2   X3   X4   X5   X6   X7   X8   X9   XA   XB   XC   CD   XE   XF */
    /* 0X */  BRK, ORA, NOP, NOP, NOP, ORA, ASL, NOP, PHP, ORA, ASL, NOP, NOP, ORA, ASL, NOP,
    /* 1X */  BPL, ORA, NOP, NOP, NOP, ORA, ASL, NOP, CLC, ORA, NOP, NOP, NOP, ORA, ASL, NOP,
    /* 2X */  JSR, AND, NOP, NOP, BIT, AND, ROL, NOP, PLP, AND, ROL, NOP, BIT, AND, ROL, NOP,
    /* 3X */  BMI, AND, NOP, NOP, NOP, AND, ROL, NOP, SEC, AND, NOP, NOP, NOP, AND, ROL, NOP,
    /* 4X */  RTI, EOR, NOP, NOP, NOP, EOR, LSR, NOP, PHA, EOR, LSR, NOP, JMP, EOR, LSR, NOP,
    /* 5X */  BVC, EOR, NOP, NOP, NOP, EOR, LSR, NOP, CLI, EOR, NOP, NOP, NOP, EOR, LSR, NOP,
    /* 6X */  RTS, ADC, NOP, NOP, NOP, ADC, ROR, NOP, PLA, ADC, ROR, NOP, JMP, ADC, ROR, NOP,
    /* 7X */  BVS, ADC, NOP, NOP, NOP, ADC, ROR, NOP, SEI, ADC, NOP, NOP, NOP, ADC, ROR, NOP,
    /* 8X */  NOP, STA, NOP, NOP, STY, STA, STX, NOP, DEY, NOP, TXA, NOP, STY, STA, STX, NOP,
    /* 9X */  BCC, STA, NOP, NOP, STY, STA, STX, NOP, TYA, STA, TXS, NOP, NOP, STA, NOP, NOP,
    /* AX */  LDY, LDA, LDX, NOP, LDY, LDA, LDX, NOP, TAY, LDA, TAX, NOP, LDY, LDA, LDX, NOP,
    /* BX */  BCS, LDA, NOP, NOP, LDY, LDA, LDX, NOP, CLV, LDA, TSX, NOP, LDY, LDA, LDX, NOP,
    /* CX */  CPY, CMP, NOP, NOP, CPY, CMP, DEC, NOP, INY, CMP, DEX, NOP, CPY, CMP, DEC, NOP,
    /* DX */  BNE, CMP, NOP, NOP, NOP, CMP, DEC, NOP, CLD, CMP, NOP, NOP, NOP, CMP, DEC, NOP,
    /* EX */  CPX, SBC, NOP, NOP, CPX, SBC, INC, NOP, INX, SBC, NOP, NOP, CPX, SBC, INC, NOP,
    /* FX */  BEQ, SBC, NOP, NOP, NOP, SBC, INC, NOP, SED, SBC, NOP, NOP, NOP, SBC, INC, NOP,
    };

    byte address_mode_table[0x100] = {
    /*        X0   X1   X2   X3   X4   X5   X6   X7   X8   X9   XA   XB   XC   CD   XE   XF */
    /* 0X */  imp, xin, NOA, NOA, NOA, zpg, zpg, NOA, imp, imm, acc, NOA, NOA, abs, abs, NOA,
    /* 1X */  rel, iny, NOA, NOA, NOA, zpx, zpx, NOA, imp, aby, NOA, NOA, NOA, abx, abx, NOA,
    /* 2X */  abs, xin, NOA, NOA, zpg, zpg, zpg, NOA, imp, imm, acc, NOA, abs, abs, abs, NOA,
    /* 3X */  rel, iny, NOA, NOA, NOA, zpx, zpx, NOA, imp, aby, NOA, NOA, NOA, abx, abx, NOA,
    /* 4X */  imp, xin, NOA, NOA, NOA, zpg, zpg, NOA, imp, imm, acc, NOA, abs, abs, abs, NOA,
    /* 5X */  rel, iny, NOA, NOA, NOA, zpx, zpx, NOA, imp, aby, NOA, NOA, NOA, abx, abx, NOA,
    /* 6X */  imp, xin, NOA, NOA, NOA, zpg, zpg, NOA, imp, imm, acc, NOA, ind, abs, abs, NOA,
    /* 7X */  rel, iny, NOA, NOA, NOA, zpx, zpx, NOA, imp, aby, NOA, NOA, NOA, abx, abx, NOA,
    /* 8X */  NOA, xin, NOA, NOA, zpg, zpg, zpg, NOA, imp, NOA, imp, NOA, abs, abs, abs, NOA,
    /* 9X */  rel, iny, NOA, NOA, zpx, zpx, zpy, NOA, imp, aby, imp, NOA, NOA, abx, NOA, NOA,
    /* AX */  imm, xin, imm, NOA, zpg, zpg, zpg, NOA, imp, imm, imp, NOA, abs, abs, abs, NOA,
    /* BX */  rel, iny, NOA, NOA, zpx, zpx, zpy, NOA, imp, aby, imp, NOA, abx, abx, aby, NOA,
    /* CX */  imm, xin, NOA, NOA, zpg, zpg, zpg, NOA, imp, imm, imp, NOA, abs, abs, abs, NOA,
    /* DX */  rel, iny, NOA, NOA, NOA, zpx, zpx, NOA, imp, aby, NOA, NOA, NOA, abx, abx, NOA,
    /* EX */  imm, xin, NOA, NOA, zpg, zpg, zpg, NOA, imp, imm, imp, NOA, abs, abs, abs, NOA,
    /* FX */  rel, iny, NOA, NOA, NOA, zpx, zpx, NOA, imp, aby, NOA, NOA, NOA, abx, abx, NOA,
    };


  public:
    CPU();
    int execute_instruction(byte opcode);
    int run(int min_cycles);
    void log_current_state();
    void log_hex_dump(int start, int stop);
    Memory memory = Memory(0xFFFF);
    Memory * get_memory_instance();
};