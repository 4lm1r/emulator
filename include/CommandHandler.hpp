#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include "Registers.hpp"  // For Registers
#include "Memory.hpp"     // For Memory
#include <functional>
#include <map>
#include <string>

// Forward declaration of CPU to avoid circular dependency
class CPU;

class CommandHandler {
public:
    CommandHandler(CPU& cpu_ref);  // Constructor declaration
    std::string executeCommand(const std::string& cmd, uint32_t* memory_start_addr);

private:
    CPU& cpu;          // Reference to CPU
    Registers& regs;   // Reference to CPU's registers
    Memory& mem;       // Reference to CPU's memory
    std::map<std::string, std::function<std::string(const std::string&, uint32_t*)>> commandMap;

    // Command functions (unchanged)
    std::string cmdMov(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdMovb(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdAdd(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdXor(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdSub(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdCmp(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdPush(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdPop(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdJe(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdJne(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdJg(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdJl(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdJge(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdJle(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdRun(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdClear(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdMemset(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdSettext(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdMemview(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdHelp(const std::string& cmd, uint32_t* memory_start_addr);
    std::string cmdQuit(const std::string& cmd, uint32_t* memory_start_addr);

    // Helper function
    bool parseMemoryAddress(const std::string& arg, std::string& reg_out, int32_t& offset_out);
};

#endif
