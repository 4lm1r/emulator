#include "Emulator.hpp"
#include <unistd.h>
#include <sstream>
#include <algorithm>

Emulator::Emulator() : cpu(regs, mem), memory_start_addr(0xFFFFF000) {}

void Emulator::run() {
    screen.updateRegisters(regs.getAll(), "");
    screen.updateStack(mem, regs.get("ESP"));
    screen.updateMemoryAndHistory(mem.getAll(), memory_start_addr, cpu.getHistory());
    screen.updateStatus("Ready (Enter to submit)");

    while (true) {
        std::string input = screen.getInput();
        if (!input.empty()) {
            std::string op;
            std::stringstream ss(input);
            ss >> op;
            std::string op_upper = op;
            std::transform(op_upper.begin(), op_upper.end(), op_upper.begin(), ::toupper);

            std::string status = cpu.execute(input, (op_upper == "MEMSET") ? &memory_start_addr : nullptr);
            if (op_upper == "MEMSET") { // Debug only for MEMSET
                auto mem_map = mem.getAll();
                char debug_str[128];
                snprintf(debug_str, sizeof(debug_str), " | Pre-display: [%02x %02x %02x %02x %02x %02x]",
                         mem_map.count(0x100) ? mem_map.at(0x100) : 0,
                         mem_map.count(0x101) ? mem_map.at(0x101) : 0,
                         mem_map.count(0x102) ? mem_map.at(0x102) : 0,
                         mem_map.count(0x103) ? mem_map.at(0x103) : 0,
                         mem_map.count(0x104) ? mem_map.at(0x104) : 0,
                         mem_map.count(0x105) ? mem_map.at(0x105) : 0);
                status += debug_str;
            }
            screen.updateStatus(status);
            screen.updateRegisters(regs.getAll(), "");
            screen.updateStack(mem, regs.get("ESP"));
            screen.updateMemoryAndHistory(mem.getAll(), memory_start_addr, cpu.getHistory());

            if (status == "QUIT") {
                break; // Exit on QUIT
            }

            // Handle special commands
            if (op_upper == "HELP") {
                screen.updateStatus("Commands: MOV Rn Rm/val/[Rm+off] or [Rm+off]/[addr] Rn/val, MOVB R8 [Rm+off]/[addr] or [Rm+off]/[addr] val, ADD/XOR/SUB/CMP Rn Rm/val or [Rm+off]/[addr] Rn, PUSH Rn, POP Rn, JE/JZ addr, JNE/JNZ addr, JG addr, JL addr, JGE addr, JLE addr, RUN, CLEAR [ALL/REGS/STACK/HISTORY], MEMSET addr, SETTEXT addr \"text\", QUIT");
            } else if (op_upper != "RUN" && op_upper != "QUIT" && status.find("failed") == std::string::npos && status.find("Unknown") == std::string::npos) {
                screen.updateStatus(status);
            }
        }
        usleep(50000); // 50ms delay for responsiveness
    }
}
/*
void Emulator::run() {
    screen.updateRegisters(regs.getAll(), ""); // Add empty changed_reg
    screen.updateStack(mem, regs.get("ESP")); // Pass Memory object directly
    screen.updateMemoryAndHistory(mem.getAll(), memory_start_addr, cpu.getHistory());
    screen.updateStatus("Ready (Enter to submit)");
    
    while (true) {
        std::string input = screen.getInput();
        if (!input.empty()) {
            std::string op;
            std::stringstream ss(input);
            ss >> op;
            std::string op_upper = op;
            std::transform(op_upper.begin(), op_upper.end(), op_upper.begin(), ::toupper);

            std::string status = cpu.execute(input, (op_upper == "MEMSET") ? &memory_start_addr : nullptr);
            if (op_upper == "MEMSET") { // Debug only for MEMSET
                auto mem_map = mem.getAll();
                char debug_str[128];
                snprintf(debug_str, sizeof(debug_str), " | Pre-display: [%02x %02x %02x %02x %02x %02x]",
                         mem_map.count(0x100) ? mem_map.at(0x100) : 0,
                         mem_map.count(0x101) ? mem_map.at(0x101) : 0,
                         mem_map.count(0x102) ? mem_map.at(0x102) : 0,
                         mem_map.count(0x103) ? mem_map.at(0x103) : 0,
                         mem_map.count(0x104) ? mem_map.at(0x104) : 0,
                         mem_map.count(0x105) ? mem_map.at(0x105) : 0);
                status += debug_str;
            }
            screen.updateStatus(status);
            screen.updateRegisters(regs.getAll(), "");
            screen.updateStack(mem, regs.get("ESP"));
            screen.updateMemoryAndHistory(mem.getAll(), memory_start_addr, cpu.getHistory());
            }
    
      while (true) {
        std::string input = screen.getInput();
        if (!input.empty()) {
            std::string op;
            std::stringstream ss(input);
            ss >> op;
            std::string op_upper = op;
            std::transform(op_upper.begin(), op_upper.end(), op_upper.begin(), ::toupper);

            std::string status = cpu.execute(input, (op_upper == "MEMSET") ? &memory_start_addr : nullptr);
            auto mem_map = mem.getAll();
            char debug_str[128];
            snprintf(debug_str, sizeof(debug_str), "Pre-display: [%02x %02x %02x %02x %02x %02x]",
                     mem_map.count(0x100) ? mem_map.at(0x100) : 0,
                     mem_map.count(0x101) ? mem_map.at(0x101) : 0,
                     mem_map.count(0x102) ? mem_map.at(0x102) : 0,
                     mem_map.count(0x103) ? mem_map.at(0x103) : 0,
                     mem_map.count(0x104) ? mem_map.at(0x104) : 0,
                     mem_map.count(0x105) ? mem_map.at(0x105) : 0);
            screen.updateStatus(status + " | " + debug_str); // Ncurses-safe
            screen.updateRegisters(regs.getAll(), "");
            screen.updateStack(mem, regs.get("ESP"));
            screen.updateMemoryAndHistory(mem.getAll(), memory_start_addr, cpu.getHistory());
            */
    /*while (true) {
        std::string input = screen.getInput();
        if (!input.empty()) {
            std::string op;
            std::stringstream ss(input);
            ss >> op;
            std::string op_upper = op;
            std::transform(op_upper.begin(), op_upper.end(), op_upper.begin(), ::toupper);

            std::string status = cpu.execute(input, (op_upper == "MEMSET") ? &memory_start_addr : nullptr);
            screen.updateStatus(status);
            screen.updateRegisters(regs.getAll(), ""); // Add empty changed_reg
            screen.updateStack(mem, regs.get("ESP")); // Pass Memory object directly
            screen.updateMemoryAndHistory(mem.getAll(), memory_start_addr, cpu.getHistory());
            
            if (status == "QUIT") {
                break; // Exit on QUIT
            }

            // Handle special commands
            if (op_upper == "HELP") {
                screen.updateStatus("Commands: MOV Rn Rm/val/[Rm+off] or [Rm+off]/[addr] Rn/val, MOVB R8 [Rm+off]/[addr] or [Rm+off]/[addr] val, ADD/XOR/SUB/CMP Rn Rm/val or [Rm+off]/[addr] Rn, PUSH Rn, POP Rn, JE/JZ addr, JNE/JNZ addr, JG addr, JL addr, JGE addr, JLE addr, RUN, CLEAR [ALL/REGS/STACK/HISTORY], MEMSET addr, SETTEXT addr \"text\", QUIT");
            } else if (op_upper != "RUN" && op_upper != "QUIT" && status.find("failed") == std::string::npos && status.find("Unknown") == std::string::npos) {
                screen.updateStatus(status);
            }
        }
        usleep(50000); // 50ms delay for responsiveness
    }
}  */
