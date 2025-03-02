#ifndef CPU_HPP
#define CPU_HPP

#include "Registers.hpp"
#include "Memory.hpp"
#include <string>
#include <vector>
#include <utility>

// Forward declaration of CommandHandler
class CommandHandler;

class CPU {
public:
    CPU(Registers& r, Memory& m);
    ~CPU();  // Add destructor
    std::string execute(const std::string& cmd, uint32_t* memory_start_addr);
    std::vector<std::pair<uint32_t, std::string>>& getHistory();
    void clearHistory();
    void runHistory();

    Registers& regs;
    Memory& mem;
    bool is_running;

    static const uint32_t ZF = 0x40;  // Zero Flag
    static const uint32_t SF = 0x80;  // Sign Flag
    static const uint32_t OF = 0x800; // Overflow Flag
    static const uint32_t PROGRAM_BASE = 0x1000;

private:
    std::vector<std::pair<uint32_t, std::string>> history;
    CommandHandler* commandHandler;

    std::string memview(uint32_t addr, const std::string& addr_str, uint32_t* memory_start_addr);

    // Declare CommandHandler as a friend class
    friend class CommandHandler;
};

#endif
