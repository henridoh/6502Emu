#include "datatypes.h"
using namespace data_types;

#include "cpu.h"

int CPU::execute_instruction(byte opcode){
  byte address_mode = address_mode_table[opcode];
  word src;
  switch (address_mode)
  {
  case acc:
    src = get_a();
    break;
  case abs:
    src = get_abs();
    break;
  case abx:
    src = get_abs_x();
    break;
  case aby:
    src = get_abs_y();
    break;
  case imm:
    src = get_imm();
    break;
  case imp:
    src = get_imp();
    break;
  case ind:
    src = get_ind();
    break;
  case xin:
    src = get_x_ind();
    break;
  case iny:
    src = get_ind_y();
    break;
  case rel:
    src = get_rel();
    break;
  case zpg:
    src = get_zpg();
    break;
  case zpx:
    src = get_zpg_x();
    break;
  case zpy:
    src = get_zpg_y();
    break;
  case NOA:
    src = get_noa();
    break;
  
  default:
    break;
  }

  switch(instruction_table[opcode]){

    case ADC: {
      byte m = memory.read(src);
      unsigned int tmp = m + reg.A + (is_carry_set() ? 1 : 0);
      set_zero(!(tmp & 0xFF));
      if (is_decimal_set())
      {
        if (((reg.A & 0xF) + (m & 0xF) + (is_carry_set() ? 1 : 0)) > 9) tmp += 6;
        set_negative(tmp & 0x80);
        set_overflow(!((reg.A ^ m) & 0x80) && ((reg.A ^ tmp) & 0x80));
        if (tmp > 0x99)
        {
          tmp += 96;
        }
        set_carry(tmp > 0x99);
      }
      else
      {
        set_negative(tmp & 0x80);
        set_overflow(!((reg.A ^ m) & 0x80) && ((reg.A ^ tmp) & 0x80));
        set_carry(tmp > 0xFF);
      }

      reg.A = tmp & 0xFF;
      break;
    }
    
    // A = A & M
    case AND: {
      byte m = memory.read(src);
      reg.A &= m;
      set_negative(reg.A & 0x80);
      set_zero(!reg.A);
      break; 
    }

    case ASL: {
      switch(address_mode){
        case acc: {
          byte m = reg.A;
          set_carry(m & 0x01);
          m <<= 1;
          set_zero(!m);
          set_negative(0);
          reg.A = m;
          break;
        }
        default: {
          byte m = memory.read(src);
          set_carry(m & 0x80);
          m <<= 1;
          set_negative(m & 0x80);
          set_zero(!m);
          memory.write(src, m);
          break;
        }
      }
      break;
    }

    case BCC: {
      if (!is_carry_set()){
        reg.PC = src;
      }
      break;
    }

    case BCS: {
      if(is_carry_set()){
        reg.PC = src;
      }
      break;
    }

    case BEQ: {
      if (is_zero_set()){
        reg.PC = src;
      }
      break;
    }

    case BIT: {
      byte m = memory.read(src);
      byte result = m & reg.A;
      set_negative(result & 0x80);
      reg.P = (reg.P & ~0b11000000) | byte(m & 0b11000000);
      break;
    }

    case BMI: {
      if(is_negative_set()){
        reg.PC = src;
      }
      break;
    }

    case BNE: {
      if(!is_zero_set()){
        reg.PC = src;
      }
      break;
    }

    case BPL: {
      if(!is_negative_set()){
        reg.PC = src;
      }
      break;
    }

    case BRK: {
      interrupts.IRQ = 1;
      set_break();
      break;
    }

    case BVC: {
      if(!is_overflow_set()){
        reg.PC = src;
      }
      break;
    }

    case BVS: {
      if(is_overflow_set()){
        reg.PC = src;
      }
      break;
    }

    case CLC: {
      clear_carry();
      break;
    }

    case CLD: {
      clear_decimal();
      break;
    }

    case CLI: {
      clear_irq_disable();
      break;
    }

    case CLV: {
      clear_overflow();
      break;
    }

    case CMP: {
      word tmp = reg.A - memory.read(src);
      set_carry(tmp < 0x100);
      set_negative(tmp & 0x80);
      set_zero(!(tmp & 0xFF));
      break;
    }

    case CPX: {
      word tmp = reg.X - memory.read(src);
      set_carry(tmp < 0x100);
      set_negative(tmp & 0x80);
      set_zero(!(tmp & 0xFF));
      break;
    }

    case CPY: {
      word tmp = reg.Y - memory.read(src);
      set_carry(tmp < 0x100);
      set_negative(tmp & 0x80);
      set_zero(!(tmp & 0xFF));
      break;
    }

    case DEC: {
      byte m = memory.read(src);
      m = m - 1;
      set_negative(m & 0x80);
      set_zero(!m);
      memory.write(src, m);
      break;
    }

    case DEX: {
      reg.X -= 1;
      set_negative(reg.X & 0x80);
      set_zero(!reg.X);
      break;
    }
    
    case DEY: {
      reg.Y -= 1;
      set_negative(reg.X & 0x80);
      set_zero(!reg.X);
      break;
    }
  
    case EOR: {
      reg.A ^= memory.read(src);
      set_negative(reg.A & 0x80);
      set_zero(!reg.A);
      break;
    }

    case INC: {
      byte m = memory.read(src);
      m = m + 1;
      set_negative(m & 0x80);
      set_zero(!m);
      memory.write(src, m);
      break;
    }

    case INX: {
      reg.X += 1;
      set_negative(reg.X & 0x80);
      set_zero(!reg.X);
      break;
    }

    case INY: {
      reg.Y += 1;
      set_negative(reg.X & 0x80);
      set_zero(!reg.X);
      break;
    }

    case JMP: {
      reg.PC = src;
      break;
    }

    case JSR: {
	    push((--reg.PC >> 8) & 0xFF);
	    push(reg.PC & 0xFF);
	    reg.PC = src;
      break;
    }

    case LDA: {
      byte m = memory.read(src);
      set_negative(m & 0x80);
      set_zero(!m);
      reg.A = m;
      break;
    }

    case LDX: {
      byte m = memory.read(src);
      set_negative(m & 0x80);
      set_zero(!m);
      reg.X = m;
      break;
    }

    case LDY: {
      byte m = memory.read(src);
      set_negative(m & 0x80);
      set_zero(!m);
      reg.Y = m;
      break;
    }

    case LSR: {
      switch(address_mode){
        case acc: {
          byte m = reg.A;
          set_carry(m & 0x01);
          m >>= 1;
          set_zero(!m);
          set_negative(0);
          reg.A = m;
          break;
        }

        default: {
          byte m = memory.read(src);
          set_carry(m & 0x01);
          m >>= 1;
          set_negative(0);
          set_zero(!m);
          memory.write(src, m);
          break;
        }
      }
      break;
    }

    case NOP: {
      break;
    }

    case ORA: {
      byte m = memory.read(src);
      m |= reg.A;
      set_negative(m & 0x80);
      set_zero(!m);
      reg.A = m;
      break;
    }

    case PHA: {
      push(reg.A);
      break;
    }

    case PHP: {
      push(reg.P);
      break;
    }

    case PLA: {
      reg.A = pop();
      set_negative(reg.A & 0x80);
      set_zero(!reg.A);
      break;
    }

    case PLP: {
      reg.P = pop();
      break;
    }

    case ROL: {
      switch (address_mode){
        case acc: {
          word m = reg.A;
          m <<= 1;
          if (is_carry_set())
            m |= 0x01;
          set_carry(m > 0xFF);
          m &= 0xFF;
          set_negative(m & 0x80);
          set_zero(!m);
          reg.A = m;
          break;
        }

        default: {
          word m = memory.read(src);
          m <<= 1;
          if(is_carry_set()) 
            m |= 1;
          set_carry(m > 0xFF);
          m &= 0xFF;
          set_negative(m & 0x80);
          set_zero(!m);
          memory.write(src, m);
          break;
        }
      }
      break;
    }

    case ROR: {
      switch (address_mode){
        case acc: {
          word m = reg.A;
          m >>= 1;
          if (is_carry_set())
            m |= 0x01;
          set_carry(m > 0xFF);
          m &= 0xFF;
          set_negative(m & 0x80);
          set_zero(!m);
          reg.A = m;
          break;
        }

        default: {
          word m = memory.read(src);
          m >>= 1;
          if(is_carry_set()) 
            m |= 1;
          set_carry(m > 0xFF);
          m &= 0xFF;
          set_negative(m & 0x80);
          set_zero(!m);
          memory.write(src, m);
          break;
        }
      }
      break;
    }

    case RTI: {
      reg.P = pop();
      reg.PC = word(pop()) | (word(pop()) << 8);
      break;
    }

    case RTS: {
      reg.P = pop();
      reg.PC = word(pop()) | (word(pop()) << 8) + 1;
      break;
    }

    case SBC: {
      byte m = memory.read(src);
      word tmp = reg.A - m - (is_carry_set() ? 0 : 1);
      set_negative(tmp & 0x80);
      set_zero(!(tmp & 0xFF));
      set_overflow(((reg.A ^ tmp) & 0x80) && ((reg.A ^ m) & 0x80));

      if (is_decimal_set())
      {
        if ( ((reg.A & 0x0F) - (is_carry_set() ? 0 : 1)) < (m & 0x0F)) tmp -= 6;
        if (tmp > 0x99)
          tmp -= 0x60;
      }
      set_carry(tmp < 0x100);
      reg.A = byte(tmp & 0xFF);
      break;
    }

    case SEC: {
      set_carry();
      break;
    }

    case SED: {
      set_decimal();
      break;
    }

    case SEI: {
      set_irq_disable();
      break;
    }

    case STA: {
      memory.write(src, reg.A);
      break;
    }

    case STX: {
      memory.write(src, reg.X);
      break;
    }

    case STY: {
      memory.write(src, reg.Y);
      break;
    }

    case TAX: {
      reg.X = reg.A;
      set_negative(reg.X & 0x80);
      set_zero(!reg.X);
      break;
    }

    case TAY: {
      reg.Y = reg.A;
      set_negative(reg.Y & 0x80);
      set_zero(!reg.Y);
      break;
    }

    case TSX: {
      reg.X = reg.S;
      set_negative(reg.X & 0x80);
      set_zero(!reg.X);
      break;
    }

    case TXA: {
      reg.A = reg.X;
      set_negative(reg.X & 0x80);
      set_zero(!reg.X);
      break;
    }

    case TXS: {
      reg.S = reg.X;
      set_negative(reg.X & 0x80);
      set_zero(!reg.X);
      break;
    }

    case TYA: {
      reg.A = reg.Y;
      set_negative(reg.Y & 0x80);
      set_zero(!reg.Y);
      break;
    }
  }
  return 0;
}


/* Addressing modes:
 * A			Accumulator OPC A	 	    operand is AC (implied single byte instruction)
 * abs		absolute	  OPC $LLHH	 	operand is address $HHLL *
 * abs,X	absolute,   X-indexed	 	OPC $LLHH,X	 	operand is address; effective address is address incremented by X with carry **
 * abs,Y	absolute,   Y-indexed	 	OPC $LLHH,Y	 	operand is address; effective address is address incremented by Y with carry **
 * #			immediate	  OPC #$BB	 	operand is byte BB
 * impl		implied	 	  OPC	 	      operand implied
 * ind		indirect	  OPC ($LLHH)	operand is address; effective address is contents of word at address: C.w($HHLL)
 * X,ind	X-indexed,  indirect	 	OPC ($LL,X)	 	operand is zeropage address; effective address is word in (LL + X, LL + X + 1), inc. without carry: C.w($00LL + X)
 * ind,Y	indirect,   Y-indexed	 	OPC ($LL),Y	 	operand is zeropage address; effective address is word in (LL, LL + 1) incremented by Y with carry: C.w($00LL) + Y
 * rel		elative	 	  OPC $BB	 	  branch target is PC + signed offset BB ***
 * zpg		eropage	 	  OPC $LL	 	  operand is zeropage address (hi-byte is zero, address = $00LL)
 * zpg,X	zeropage,   X-indexed	 	OPC $LL,X	 	operand is zeropage address; effective address is address incremented by X without carry **
 * zpg,Y	zeropage,   Y-indexed	 	OPC $LL,Y	 	operand is zeropage address; effective address is address incremented by Y without carry **
 */
word CPU::get_a(){ return 0; }
word CPU::get_imp(){ return 0; }

word CPU::get_abs(){ return fetch_word(); } // ret word[pc] ; pc += 2
word CPU::get_abs_x(){ return fetch_word() + reg.X; } // ret word[pc] + x ; pc += 2
word CPU::get_abs_y(){ return fetch_word() + reg.Y; } // ret word[pc] + y ; pc += 2

word CPU::get_imm(){ return reg.PC++; } // ret byte[pc] ; pc++

word CPU::get_ind(){
  word abs = word(fetch_byte()) | (word(fetch_byte()) << 8);
  return word(memory.read(abs)) | (word(memory.read(abs + 1)) << 8);
}
word CPU::get_x_ind(){
  byte b = fetch_byte() + reg.X;
  return word(memory.read(b)) | word((memory.read(b + 1)) << 8);
}
word CPU::get_ind_y(){
  byte b = fetch_byte();
  return (word(memory.read(b)) |  (word(memory.read(b+1)) << 8)) + reg.Y;
}

word CPU::get_rel(){
  word offset;
	word addr;

	offset = (word)memory.read(reg.PC++);
	if (offset & 0x80) offset |= 0xFF00;
	addr = reg.PC + (sword)offset;
	return addr;;
}

word CPU::get_zpg(){
  return word(fetch_byte());
}
word CPU::get_zpg_x(){
  return (fetch_byte() + reg.X) & 0xFF;
}
word CPU::get_zpg_y(){
  return (fetch_byte() + reg.Y) & 0xFF;
}

word CPU::get_noa(){ return 0; }


// add operation
byte CPU::add_co(byte a, byte b){
  word c;
  c = a + b;
  set_carry(c & 0x100);
  set_overflow((~(a ^ b)) & (1 << 7) && byte(c & 0xFF) & (1 << 7) == a & (1 << 7));
  return byte(c & 0xFF);
}