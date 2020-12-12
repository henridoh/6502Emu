#include "datatypes.h"
using namespace data_types;

#define DEFAULT_SIZE 0xFFFF

class Memory{  
  private:
    int memory_size;
    byte * memory_array;
  public:
    Memory();
    Memory(int size);

    byte read(int address);
    void write(int address, byte value);

    void insert(int start, int len, byte data[]);
};