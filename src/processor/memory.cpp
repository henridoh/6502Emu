#include "datatypes.h"
using namespace data_types;
#include "memory.h"

Memory::Memory(){
  memory_array = new byte[DEFAULT_SIZE]{0};
  memory_size = DEFAULT_SIZE;
}

Memory::Memory(int size){
  memory_array = new byte[size]{0};
  memory_size = size;
}

void Memory::write(int address, byte value){
  if(address >= 0 && address < memory_size){
    memory_array[address] = value;
  }
}

byte Memory::read(int address){
  if(address >= 0 && address < memory_size){
    return memory_array[address];
  }
  return 0;
}

void Memory::insert(int start, int len, byte data[]){
  for(int i=0; i < len; i++){
    memory_array[start + i] = data[i];
  }
}