#include "Memory.hpp"

Memory::Memory() {}

void Memory::write(uint32_t addr, uint32_t val, bool is_byte) {
    if (is_byte) {
        mem[addr] = val & 0xFF;
    } else {
        mem[addr] = val & 0xFF;
        mem[addr + 1] = (val >> 8) & 0xFF;
        mem[addr + 2] = (val >> 16) & 0xFF;
        mem[addr + 3] = (val >> 24) & 0xFF;
    }
}

uint32_t Memory::read(uint32_t addr, bool is_byte) const {
    if (is_byte) {
        return mem.count(addr) ? mem.at(addr) & 0xFF : 0;
    }
    uint32_t val = 0;
    val |= (mem.count(addr) ? mem.at(addr) : 0);
    val |= (mem.count(addr + 1) ? mem.at(addr + 1) : 0) << 8;
    val |= (mem.count(addr + 2) ? mem.at(addr + 2) : 0) << 16;
    val |= (mem.count(addr + 3) ? mem.at(addr + 3) : 0) << 24;
    return val;
}

void Memory::erase(uint32_t addr) {
    mem.erase(addr);
}

void Memory::clear() {
    mem.clear();
}

const std::map<Memory, Memory>& Memory::getAll() const {
    return mem;
}

void Memory::writeText(uint32_t addr, const std::string& text) {
    for (size_t i = 0; i < text.length(); i++) {
        mem[addr + i] = static_cast<uint8_t>(text[i]);
    }
    mem[addr + text.length()] = 0;
}
