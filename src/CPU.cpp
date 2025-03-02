#include "CPU.hpp"
#include "CommandHandler.hpp"
#include <sstream>

CPU::CPU(Registers& r, Memory& m) : regs(r), mem(m), is_running(false), commandHandler(new CommandHandler(*this)) {
    regs.set("EIP", PROGRAM_BASE);
}

CPU::~CPU() {
    delete commandHandler;
}
std::string CPU::execute(const std::string& cmd, uint32_t* memory_start_addr) {
    return commandHandler->executeCommand(cmd, memory_start_addr);
}

std::string CPU::memview(uint32_t addr, const std::string& addr_str, uint32_t* memory_start_addr) {
    char debug_str[128];
    snprintf(debug_str, sizeof(debug_str),
             "MEMVIEW: View set to %08X: [%02x %02x %02x %02x %02x %02x]",
             addr,
             mem.read(addr, true), mem.read(addr + 1, true), mem.read(addr + 2, true),
             mem.read(addr + 3, true), mem.read(addr + 4, true), mem.read(addr + 5, true));
    if (memory_start_addr) *memory_start_addr = addr;
    uint32_t cmd_addr = regs.get("EIP");
    std::string command = "MEMVIEW " + addr_str;
    history.push_back(std::make_pair(cmd_addr, command));
    if (!is_running) regs.set("EIP", cmd_addr + 4);
    return debug_str;
}

std::vector<std::pair<uint32_t, std::string>>& CPU::getHistory() {
    return history;  // Return vector of address-command pairs
}

// Clears command history
void CPU::clearHistory() {
    history.clear();  // Empty the history vector
}

void CPU::runHistory() {
    // Unchanged
}
