#include "Registers.hpp"
#include <algorithm>
#include <cstdint>

Registers::Registers() {
    regs = {
        {"EAX", 0}, {"EBX", 0}, {"ECX", 0}, {"EDX", 0},
        {"ESI", 0}, {"EDI", 0}, {"ESP", 0xFFFFFFF0}, {"EBP", 0},
        {"AX", 0}, {"BX", 0}, {"CX", 0}, {"DX", 0},
        {"SI", 0}, {"DI", 0}, {"SP", 0xFFF0}, {"BP", 0},
        {"AH", 0}, {"AL", 0}, {"BH", 0}, {"BL", 0},
        {"CH", 0}, {"CL", 0}, {"DH", 0}, {"DL", 0},
        {"CS", 0}, {"DS", 0}, {"SS", 0}, {"ES", 0},
        {"EIP", 0}, {"IP", 0}, {"FLAGS", 0}
    };
}

uint32_t Registers::get(const std::string& reg) const {
    std::string reg_upper = reg;
    std::transform(reg_upper.begin(), reg_upper.end(), reg_upper.begin(), ::toupper);
    return regs.at(reg_upper);
}

void Registers::set(const std::string& reg, uint32_t val) {
    std::string reg_upper = reg;
    std::transform(reg_upper.begin(), reg_upper.end(), reg_upper.begin(), ::toupper);
    if (reg_upper == "FLAGS" || reg_upper == "EIP") {
        regs[reg_upper] = val; // Direct set for FLAGS and EIP
    } else {
        sync(reg_upper, val); // Sync only for other registers
    }
}

void Registers::sync(const std::string& reg, uint32_t val) {
    if (reg[0] == 'E') { // 32-bit
        regs[reg] = val & 0xFFFFFFFF;
        std::string short_reg = reg.substr(1);
        std::string low_reg = short_reg.substr(0, 1) + "L";
        std::string high_reg = short_reg.substr(0, 1) + "H";
        regs[short_reg] = val & 0xFFFF;
        regs[low_reg] = val & 0xFF;
        regs[high_reg] = (val >> 8) & 0xFF;
    } else if (reg.find('H') != std::string::npos || reg.find('L') != std::string::npos) { // 8-bit
        std::string base = reg.substr(0, 1);
        uint32_t full_val = regs["E" + base + "X"];
        if (reg[1] == 'L') {
            full_val = (full_val & 0xFFFFFF00) | (val & 0xFF);
        } else {
            full_val = (full_val & 0xFFFF00FF) | ((val & 0xFF) << 8);
        }
        regs["E" + base + "X"] = full_val;
        regs[base + "X"] = full_val & 0xFFFF;
        regs[base + "L"] = full_val & 0xFF;
        regs[base + "H"] = (full_val >> 8) & 0xFF;
    } else { // 16-bit
        regs[reg] = val & 0xFFFF;
        regs["E" + reg] = (regs["E" + reg] & 0xFFFF0000) | (val & 0xFFFF);
        std::string low_reg = reg.substr(0, 1) + "L";
        std::string high_reg = reg.substr(0, 1) + "H";
        regs[low_reg] = val & 0xFF;
        regs[high_reg] = (val >> 8) & 0xFF;
    }
}

const std::map<std::string, uint32_t>& Registers::getAll() const {
    return regs;
}
