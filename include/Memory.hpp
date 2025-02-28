#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <map>
#include <string>
#include <cstdint>

class Memory {
public:
    Memory(uint32_t value = 0);
    uint32_t getValue() const;
    void setValue(uint32_t value);
    static const uint32_t STACK_TOP = 0xFFFFFFF0;  // Top of stack (unchanged)
    static const uint32_t STACK_BASE = 0xFF000000; // Bottom of stack (expanded)
    void write(uint32_t addr, uint32_t val, bool is_byte = false);
    uint32_t read(uint32_t addr, bool is_byte = false) const;
    void erase(uint32_t addr);
    void clear();
    std::map<uint32_t, uint32_t> getAll() const;
    void writeText(uint32_t addr, const std::string& text);

private:
    uint32_t value_;
    std::map<uint32_t, uint32_t> mem;
    std::map<uint32_t, Memory> memory_;
};

#endif
