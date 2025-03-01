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
        std::string input = screen.getInput();  // Retrieve user input from the screen/UI
        if (!input.empty()) {  // Process only if input is provided
            std::string op;  // Variable to store the operation/command
            std::stringstream ss(input);  // Create a string stream to parse input
            ss >> op;  // Extract the first word (operation) from input
            std::string op_upper = op;  // Copy operation for uppercase conversion
            std::transform(op_upper.begin(), op_upper.end(), op_upper.begin(), ::toupper);  // Convert operation to uppercase for case-insensitive comparisons

            // Execute the command via CPU, passing memory_start_addr for MEMSET/MEMVIEW commands
            std::string status = cpu.execute(input, (op_upper == "MEMSET" || op_upper == "MEMVIEW") ? &memory_start_addr : nullptr);

            // Post-execution memory map check at address 0x100
            auto mem_map_after_execute = mem.getAllBytes();  // Get all memory bytes after execution
            std::string mem_check_execute = " | MemMap after execute = " + 
                                            std::to_string(mem_map_after_execute.count(0x100) ? mem_map_after_execute.at(0x100) : 0);  // Check value at 0x100, default to 0 if not present

            // Special handling for MEMSET command: display memory contents for debugging
            if (op_upper == "MEMSET") {
                auto mem_map = mem.getAllBytes();  // Get memory map
                char debug_str[128];  // Buffer for debug string
                // Format debug string with memory values at addresses 0x100 to 0x105
                snprintf(debug_str, sizeof(debug_str), " | Pre-display: [%02x %02x %02x %02x %02x %02x]",
                         mem_map.count(0x100) ? mem_map.at(0x100) : 0,  // Hex value at 0x100 or 0
                         mem_map.count(0x101) ? mem_map.at(0x101) : 0,
                         mem_map.count(0x102) ? mem_map.at(0x102) : 0,
                         mem_map.count(0x103) ? mem_map.at(0x103) : 0,
                         mem_map.count(0x104) ? mem_map.at(0x104) : 0,
                         mem_map.count(0x105) ? mem_map.at(0x105) : 0);
                status += debug_str;  // Append debug info to status
            }

            // Additional debug info: memory start address and value at 0x100
            std::string debug = " | DEBUG: memory_start_addr = " + std::to_string(memory_start_addr) +
                                " Mem at 100 = " + std::to_string(mem.read(0x100, true));  // Read memory at 0x100

            // Memory map check at 0x100 before UI update
            auto mem_map = mem.getAllBytes();
            std::string mem_check = " | MemMap at 100 = " + 
                                    std::to_string(mem_map.count(0x100) ? mem_map.at(0x100) : 0);

            // Update UI with status, memory checks, and debug info
            screen.updateStatus(status + mem_check_execute + debug + mem_check);
            screen.updateRegisters(regs.getAll(), "");  // Refresh register display
            screen.updateStack(mem, regs.get("ESP"));  // Refresh stack display
            screen.updateMemoryAndHistory(mem.getAllBytes(), memory_start_addr, cpu.getHistory());  // Refresh memory and history

            // Check for QUIT command to exit the loop
            if (status == "QUIT") {
                break;  // Exit the emulator loop
            }

            // Display help text if HELP command is issued
            if (op_upper == "HELP") {
                screen.updateStatus("Commands: MOV Rn Rm/val/[Rm+off] or [Rm+off]/[addr] Rn/val, MOVB R8 [Rm+off]/[addr] or [Rm+off]/[addr] val, ADD/XOR/SUB/CMP Rn Rm/val or [Rm+off]/[addr] Rn, PUSH Rn, POP Rn, JE/JZ addr, JNE/JNZ addr, JG addr, JL addr, JGE addr, JLE addr, RUN, CLEAR [ALL/REGS/STACK/HISTORY], MEMSET addr, SETTEXT addr \"text\", MEMVIEW addr, QUIT");
            } 
            // Update status again for non-RUN/QUIT commands if execution succeeded (no "failed" or "Unknown" in status)
            else if (op_upper != "RUN" && op_upper != "QUIT" && status.find("failed") == std::string::npos && status.find("Unknown") == std::string::npos) {
                screen.updateStatus(status + mem_check_execute + debug + mem_check);
            }
        }

        // Brief delay (50ms) to prevent excessive CPU usage
        usleep(50000);
    }
}
