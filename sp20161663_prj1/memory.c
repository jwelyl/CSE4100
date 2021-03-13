#include "memory.h"

void reset_memory() {
  int i = 0;
  for(i = 0; i < MAX_MEM_SIZE; i++)
    memory[i] = 0;
}

void edit_value(int addr, int val) {
  memory[addr] = val;
}
