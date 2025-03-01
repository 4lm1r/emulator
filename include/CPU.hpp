#ifndef CPU_HPP
#define CPU_HPP

#include "Registers.hpp"
#include "Memory.hpp"
#include <string>
#include <vector>
#include <utility>
#include <cstdint>

class CPU {
public:
    CPU(Registers& regs, Memory& mem);
    std::string execute(const std::string& cmd, uint32_t* memory_start_addr = nullptr); // Changed to string
    std::vector<std::pair<uint32_t, std::string>>& getHistory();
    void clearHistory();
    std::string memview(uint32_t addr, const std::string& addr_str, uint32_t* memory_start_addr);

private:
    Registers& regs;
    Memory& mem;
    std::vector<std::pair<uint32_t, std::string>> history;
    bool is_running;
    static const uint32_t PROGRAM_BASE = 0x1000;
    static const uint32_t ZF = 1 << 6;
    static const uint32_t SF = 1 << 7;
    static const uint32_t OF = 1 << 11;

    bool parseMemoryAddress(const std::string& arg, std::string& reg_out, int32_t& offset_out);
    void runHistory();
};

#endif
