#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include "Screen.hpp"
#include "Registers.hpp"
#include "Memory.hpp"
#include "CPU.hpp"

class Emulator {
public:
    Emulator();
    void run();

private:
    Screen screen;
    Registers regs;
    Memory mem;
    CPU cpu;
    uint32_t memory_start_addr;
};

#endif
