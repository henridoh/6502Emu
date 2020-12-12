#include <stdio.h>
#include "cpu.h"
#include "datatypes.h"
using namespace data_types;

CPU::CPU(){
}

// Stack operations (stack range: 0x100 - 0x1FF)
void CPU::push(byte value){
  memory.write(0x0100 | reg.S--, value);
}
byte CPU::pop(){
  return memory.read(0x0100 | ++reg.S);
}

byte CPU::fetch_byte(){
  return memory.read(reg.PC++);
}

word CPU::fetch_word(){
  return (word(memory.read(reg.PC++))) | (word(memory.read(reg.PC++)) << 8);
}


// flag / processor state operations
bool CPU::is_negative_set(){ return reg.P & N; }
bool CPU::is_overflow_set(){ return reg.P & V; }
bool CPU::is_break_set(){ return reg.P & B; }
bool CPU::is_decimal_set(){ return reg.P & D; }
bool CPU::is_irq_disable_set(){ return reg.P & I; }
bool CPU::is_zero_set(){ return reg.P & Z; }
bool CPU::is_carry_set(){ return reg.P & C; }

void CPU::set_negative(){ reg.P |= N; }
void CPU::set_overflow(){ reg.P |= V; }
void CPU::set_break(){ reg.P |= B; }
void CPU::set_decimal(){ reg.P |= D; }
void CPU::set_irq_disable(){ reg.P |= I; }
void CPU::set_zero(){ reg.P |= Z; }
void CPU::set_carry(){ reg.P |= C; }

void CPU::set_negative(word state){ state ? reg.P |= N : reg.P &= ~N; }
void CPU::set_overflow(word state){ state ? reg.P |= V : reg.P &= ~V; }
void CPU::set_break(word state){ state ? reg.P |= B : reg.P &= ~B; }
void CPU::set_decimal(word state){ state ? reg.P |= D : reg.P &= ~D; }
void CPU::set_irq_disable(word state){ state ? reg.P |= I : reg.P &= ~I; }
void CPU::set_zero(word state){ state ? reg.P |= Z : reg.P &= ~Z; }
void CPU::set_carry(word state){ state ? reg.P |= C : reg.P &= ~C; }

void CPU::clear_negative(){ reg.P &= ~N; }
void CPU::clear_overflow(){ reg.P &= ~V; }
void CPU::clear_break(){ reg.P &= ~B; }
void CPU::clear_decimal(){ reg.P &= ~D; }
void CPU::clear_irq_disable(){ reg.P &= ~I; }
void CPU::clear_zero(){ reg.P &= ~Z; }
void CPU::clear_carry(){ reg.P &= ~C; }

void CPU::log_current_state(){
  printf("A:\t%1$03u\t| %1$02xh\n", reg.A);
  printf("X:\t%1$03u\t| %1$02xh\n", reg.X);
  printf("Y:\t%1$03u\t| %1$02xh\n", reg.Y);
  printf("S:\t%1$03u\t| %1$02xh\n", reg.S);
  printf("PC:\t%1$05u\t| %1$04xh\n", reg.PC);
  printf("P:\t%c%c0%c%c%c%c%c\n",
         is_negative_set()    ? 'N' : '-',
         is_overflow_set()    ? 'V' : '-',
         is_break_set()       ? 'B' : '-',
         is_decimal_set()     ? 'D' : '-',
         is_irq_disable_set() ? 'I' : '-',
         is_zero_set()        ? 'Z' : '-',
         is_carry_set()       ? 'C' : '-'
        );
  printf("\n");
}


void CPU::log_hex_dump(int start, int stop){
  for(; start < stop; start++){
    if(start % 0x10 == 0){
      printf("\n0x%04x| ",start);
    }
    if (start == reg.PC) printf("\033[31m");
    else if (start == (0x0100 | reg.S)) printf("\033[32m");
    printf("%02x ", memory.read(start));
    if (start == reg.PC or start == (0x0100 | reg.S)) printf("\033[0m");
  }
  printf("\n");
}

int CPU::run(int min_cycles){
  int cycles = 0;
  byte instruction;
  while(cycles < min_cycles){
    instruction = fetch_byte();

    if(interrupts.NMI){
      interrupts.NMI = false;
      push((reg.PC >> 8) & 0xFF);
      push(reg.PC & 0xFF);
      push(reg.P);
      reg.P = word(memory.read(interrupts.NMI_VECTOR)) | (word(memory.read(interrupts.NMI_VECTOR+1) << 8));
    }
    if(interrupts.IRQ && !is_irq_disable_set()){
      if(is_break_set()){
        cycles += 100;
        reg.PC--;
        continue;
      }
      reg.PC++;
      push((reg.PC >> 8) & 0xFF);
      push(reg.PC & 0xFF);
      push(reg.P);
      reg.P = word(memory.read(interrupts.IRQ_VECTOR)) | (word(memory.read(interrupts.IRQ_VECTOR+1) << 8));
    }

    cycles += 1;
    execute_instruction(instruction);
  }

  return cycles;
}

Memory * CPU::get_memory_instance(){
  return &memory;
}