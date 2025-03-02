#include "Emulator.hpp"
#include <unistd.h>      // For usleep() to introduce delays
#include <sstream>       // For string stream processing
#include <algorithm>     // For std::transform to convert strings to uppercase

// Constructor for Emulator class
// Initializes the CPU with registers (regs) and memory (mem), sets default memory start address
Emulator::Emulator() : cpu(regs, mem), memory_start_addr(0xFFFFF000) {}

// Main execution loop for the emulator
void Emulator::run() {
    // Initial UI update: display registers, stack, memory, and status
    screen.updateRegisters(regs.getAll(), "");  // Show all register values
    screen.updateStack(mem, regs.get("ESP"));  // Update stack view using ESP (stack pointer)
    screen.updateMemoryAndHistory(mem.getAllBytes(), memory_start_addr, cpu.getHistory());  // Show memory and CPU history
    screen.updateStatus("Ready (Enter to submit)");  // Indicate emulator is ready for input

    // Infinite loop to process emulator commands
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

            // Update UI with status, memory checks, and debug info
            screen.updateStatus(status + mem_check_execute + debug + mem_check);
            screen.updateRegisters(regs.getAll(), "");  // Refresh register display
            screen.updateStack(mem, regs.get("ESP"));  // Refresh stack display
            screen.updateMemoryAndHistory(mem.getAllBytes(), memory_start_addr, cpu.getHistory());  // Refresh memory and history

            // Check for QUIT command to exit the loop
            if (status == "QUIT") {
                break;  // Exit the emulator loop
            }
        }

        // Brief delay (50ms) to prevent excessive CPU usage
        usleep(50000);
    }
}
