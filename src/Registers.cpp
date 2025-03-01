#include "Registers.hpp"
#include <algorithm>  // For std::transform to handle case-insensitive register names
#include <cstdint>    // For uint32_t type definition

// Constructor for Registers class
// Initializes a map of register names to their default values
Registers::Registers() {
    regs = {
        // 32-bit general-purpose registers initialized to 0
        {"EAX", 0}, {"EBX", 0}, {"ECX", 0}, {"EDX", 0},
        {"ESI", 0}, {"EDI", 0}, 
        {"ESP", 0xFFFFFFF0},  // Stack pointer initialized to top of stack (near 4GB)
        {"EBP", 0},           // Base pointer initialized to 0
        // 16-bit general-purpose registers (lower 16 bits of 32-bit counterparts)
        {"AX", 0}, {"BX", 0}, {"CX", 0}, {"DX", 0},
        {"SI", 0}, {"DI", 0}, 
        {"SP", 0xFFF0},       // 16-bit stack pointer initialized to lower 16 bits of ESP
        {"BP", 0},            // 16-bit base pointer
        // 8-bit registers (low and high bytes of 16-bit registers)
        {"AH", 0}, {"AL", 0}, {"BH", 0}, {"BL", 0},
        {"CH", 0}, {"CL", 0}, {"DH", 0}, {"DL", 0},
        // Segment registers initialized to 0
        {"CS", 0}, {"DS", 0}, {"SS", 0}, {"ES", 0},
        // Instruction pointer and flags
        {"EIP", 0},           // 32-bit instruction pointer
        {"IP", 0},            // 16-bit instruction pointer
        {"FLAGS", 0}          // CPU flags register
    };
}

// Retrieves the value of a specified register
uint32_t Registers::get(const std::string& reg) const {
    std::string reg_upper = reg;  // Copy register name
    std::transform(reg_upper.begin(), reg_upper.end(), reg_upper.begin(), ::toupper);  // Convert to uppercase
    return regs.at(reg_upper);  // Return the 32-bit value of the register (throws if not found)
}

// Sets the value of a specified register and synchronizes related registers
void Registers::set(const std::string& reg, uint32_t val) {
    std::string reg_upper = reg;  // Copy register name
    std::transform(reg_upper.begin(), reg_upper.end(), reg_upper.begin(), ::toupper);  // Convert to uppercase
    if (reg_upper == "FLAGS" || reg_upper == "EIP") {
        regs[reg_upper] = val;  // Directly set FLAGS or EIP without synchronization
    } else {
        sync(reg_upper, val);  // Synchronize other registers (e.g., EAX/AX/AL/AH)
    }
}

// Synchronizes register values based on size (32-bit, 16-bit, or 8-bit) and relationships
void Registers::sync(const std::string& reg, uint32_t val) {
    if (reg[0] == 'E') {  // 32-bit register (e.g., EAX, ESP)
        regs[reg] = val & 0xFFFFFFFF;  // Set full 32-bit value
        std::string short_reg = reg.substr(1);  // 16-bit version (e.g., AX, SP)
        std::string low_reg = short_reg.substr(0, 1) + "L";  // Low byte (e.g., AL)
        std::string high_reg = short_reg.substr(0, 1) + "H";  // High byte (e.g., AH)
        regs[short_reg] = val & 0xFFFF;        // Update 16-bit register
        regs[low_reg] = val & 0xFF;            // Update low 8-bit register
        regs[high_reg] = (val >> 8) & 0xFF;    // Update high 8-bit register
    } else if (reg.find('H') != std::string::npos || reg.find('L') != std::string::npos) {  // 8-bit register (e.g., AH, AL)
        std::string base = reg.substr(0, 1);  // Base register letter (e.g., A, B)
        uint32_t full_val = regs["E" + base + "X"];  // Get full 32-bit register (e.g., EAX)
        if (reg[1] == 'L') {  // Low byte (e.g., AL)
            full_val = (full_val & 0xFFFFFF00) | (val & 0xFF);  // Update only low byte
        } else {  // High byte (e://AH)
            full_val = (full_val & 0xFFFF00FF) | ((val & 0xFF) << 8);  // Update high byte
        }
        regs["E" + base + "X"] = full_val;       // Update 32-bit register
        regs[base + "X"] = full_val & 0xFFFF;    // Update 16-bit register
        regs[base + "L"] = full_val & 0xFF;      // Update low byte
        regs[base + "H"] = (full_val >> 8) & 0xFF;  // Update high byte
    } else {  // 16-bit register (e.g., AX, SP)
        regs[reg] = val & 0xFFFF;  // Set 16-bit value
        regs["E" + reg] = (regs["E" + reg] & 0xFFFF0000) | (val & 0xFFFF);  // Update lower 16 bits of 32-bit register
        std::string low_reg = reg.substr(0, 1) + "L";  // Low byte (e.g., AL)
        std::string high_reg = reg.substr(0, 1) + "H";  // High byte (e.g., AH)
        regs[low_reg] = val & 0xFF;            // Update low byte
        regs[high_reg] = (val >> 8) & 0xFF;    // Update high byte
    }
}

// Returns a const reference to the entire register map
const std::map<std::string, uint32_t>& Registers::getAll() const {
    return regs;  // Return the map of all registers and their values
}
