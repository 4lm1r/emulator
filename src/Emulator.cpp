#include "Emulator.hpp"
#include <unistd.h>
#include <sstream>
#include <algorithm>

Emulator::Emulator() : cpu(regs, mem), memory_start_addr(0xFFFFF000) {}

void Emulator::run() {
    screen.updateRegisters(regs.getAll(), "");
    screen.updateStack(mem, regs.get("ESP"));
    screen.updateMemoryAndHistory(mem.getAllBytes(), memory_start_addr, cpu.getHistory());
    screen.updateStatus("Ready (Enter to submit)");

    while (true) {
        std::string input = screen.getInput();
        if (!input.empty()) {
            std::string status = cpu.execute(input, &memory_start_addr);

            auto mem_map = mem.getAllBytes();
            std::string mem_check = " | MemMap at 100 = " +
                                    std::to_string(mem_map.count(0x100) ? mem_map.at(0x100) : 0);
            std::string debug = " | DEBUG: memory_start_addr = " + std::to_string(memory_start_addr) +
                                " Mem at 100 = " + std::to_string(mem.read(0x100, true));

            screen.updateStatus(status + mem_check + debug);
            screen.updateRegisters(regs.getAll(), "");
            screen.updateStack(mem, regs.get("ESP"));
            screen.updateMemoryAndHistory(mem.getAllBytes(), memory_start_addr, cpu.getHistory());

            if (status == "QUIT") {
                break;
            }
        }
        usleep(50000);
    }
}
