#include "CPU.hpp"
#include <sstream>      // For string stream parsing
#include <iomanip>      // For output formatting
#include <algorithm>    // For std::transform to handle case-insensitive operations
#include <set>          // For defining byte registers in MOVB
#include <unistd.h>     // For usleep() delays
#include <cstring>      // For snprintf to format debug strings

// Constructor for CPU class
// Initializes CPU with references to registers and memory, sets EIP to program base address
CPU::CPU(Registers& r, Memory& m) : regs(r), mem(m), is_running(false) {
    regs.set("EIP", PROGRAM_BASE);  // Set instruction pointer to the base address of the program
}

// Displays memory contents starting at a given address and updates memory view
std::string CPU::memview(uint32_t addr, const std::string& addr_str, uint32_t* memory_start_addr) {
    char debug_str[128];  // Buffer for debug output
    // Format memory contents at addr to addr+5 in hex
    snprintf(debug_str, sizeof(debug_str), 
             "MEMVIEW: View set to %08X: [%02x %02x %02x %02x %02x %02x]", 
             addr, 
             mem.read(addr, true), mem.read(addr + 1, true), mem.read(addr + 2, true),
             mem.read(addr + 3, true), mem.read(addr + 4, true), mem.read(addr + 5, true));
    if (memory_start_addr) *memory_start_addr = addr;  // Update memory start address if provided
    uint32_t cmd_addr = regs.get("EIP");  // Current instruction pointer
    std::string command = "MEMVIEW " + addr_str;  // Construct command string for history
    history.push_back(std::make_pair(cmd_addr, command));  // Log command in history
    if (!is_running) regs.set("EIP", cmd_addr + 4);  // Increment EIP by 4 (instruction size) if not running
    return debug_str;  // Return formatted memory view string
}

// Executes a single command and updates CPU state accordingly
std::string CPU::execute(const std::string& cmd, uint32_t* memory_start_addr) {
    std::string status;  // Status message to return
    std::string op, reg1, reg2;  // Operation and operands
    std::stringstream ss(cmd);  // Stream to parse command
    ss >> op >> reg1;  // Extract operation and first operand

    // Convert operation and first operand to uppercase for case-insensitive handling
    std::string op_upper = op;
    std::string reg1_upper = reg1;
    std::transform(op_upper.begin(), op_upper.end(), op_upper.begin(), ::toupper);
    std::transform(reg1_upper.begin(), reg1_upper.end(), reg1_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");  // Current instruction pointer

    // MOV: Move data between registers or memory
    if (op_upper == "MOV") {
        ss >> reg2;  // Extract second operand
        if (reg1[0] == '[') {  // Memory destination ([address])
            std::string mem_reg;  // Register for memory addressing
            int32_t offset;  // Offset for memory addressing
            if (parseMemoryAddress(reg1, mem_reg, offset)) {  // Parse memory address syntax
                std::string reg2_upper = reg2;
                std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
                uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);  // Compute effective address
                uint32_t val;  // Value to write
                if (regs.getAll().count(reg2_upper)) {  // Source is a register
                    val = regs.get(reg2_upper);
                    mem.write(addr, val);  // Write register value to memory
                    char debug_str[64];
                    snprintf(debug_str, sizeof(debug_str), "MOV [%08X] <- %08X", addr, val);
                    status = debug_str;
                    if (!is_running) {
                        history.push_back({cmd_addr, cmd});  // Log command
                        regs.set("EIP", cmd_addr + 4);  // Increment EIP
                    }
                } else {  // Source is immediate value
                    try {
                        val = std::stoul(reg2_upper, nullptr, 16);  // Parse hex value
                        mem.write(addr, val);
                        char debug_str[64];
                        snprintf(debug_str, sizeof(debug_str), "MOV [%08X] <- %08X", addr, val);
                        status = debug_str;
                        if (!is_running) {
                            history.push_back({cmd_addr, cmd});
                            regs.set("EIP", cmd_addr + 4);
                        }
                    } catch (...) {
                        status = "MOV failed: Invalid value";
                    }
                }
            } else {
                status = "MOV failed: Invalid memory address";
            }
        } else if (regs.getAll().count(reg1_upper)) {  // Register destination
            std::string reg2_upper = reg2;
            std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
            if (regs.getAll().count(reg2_upper)) {  // Register to register move
                uint32_t val = regs.get(reg2_upper);
                regs.set(reg1_upper, val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "MOV %s <- %08X", reg1_upper.c_str(), val);
                status = debug_str;
                if (!is_running) {
                    history.push_back({cmd_addr, cmd});
                    regs.set("EIP", cmd_addr + 4);
                }
            } else if (reg2[0] == '[') {  // Memory to register move
                std::string mem_reg;
                int32_t offset;
                if (parseMemoryAddress(reg2, mem_reg, offset)) {
                    uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                    uint32_t mem_val = mem.read(addr);  // Read value from memory
                    regs.set(reg1_upper, mem_val);
                    char debug_str[64];
                    snprintf(debug_str, sizeof(debug_str), "MOV %s <- [%08X] = %08X", 
                             reg1_upper.c_str(), addr, mem_val);
                    status = debug_str;
                    if (!is_running) {
                        history.push_back({cmd_addr, cmd});
                        regs.set("EIP", cmd_addr + 4);
                    }
                } else {
                    status = "MOV failed: Invalid memory address";
                }
            } else if (!reg2.empty()) {  // Immediate value to register
                try {
                    uint32_t val = std::stoul(reg2_upper, nullptr, 16);
                    regs.set(reg1_upper, val);
                    char debug_str[64];
                    snprintf(debug_str, sizeof(debug_str), "MOV %s <- %08X", reg1_upper.c_str(), val);
                    status = debug_str;
                    if (!is_running) {
                        history.push_back({cmd_addr, cmd});
                        regs.set("EIP", cmd_addr + 4);
                    }
                } catch (...) {
                    status = "MOV failed: Invalid value";
                }
            } else {
                status = "MOV failed: Missing operand";
            }
        } else {
            status = "MOV failed: Invalid register";
        }
    } 
    // MOVB: Move byte-sized data
    else if (op_upper == "MOVB") {
        ss >> reg2;
        if (reg1[0] == '[') {  // Memory destination
            std::string mem_reg;
            int32_t offset;
            if (parseMemoryAddress(reg1, mem_reg, offset)) {
                std::string reg2_upper = reg2;
                std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
                uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                try {
                    uint32_t val = std::stoul(reg2_upper, nullptr, 16);
                    if (val > 0xFF) {  // Check if value fits in a byte
                        status = "MOVB failed: Value exceeds byte size";
                    } else {
                        mem.write(addr, val, true);  // Write byte to memory
                        char debug_str[64];
                        snprintf(debug_str, sizeof(debug_str), "MOVB [%08X] <- %02X", addr, val);
                        status = debug_str;
                        if (!is_running) {
                            history.push_back({cmd_addr, cmd});
                            regs.set("EIP", cmd_addr + 4);
                        }
                    }
                } catch (...) {
                    status = "MOVB failed: Invalid value";
                }
            } else {
                status = "MOVB failed: Invalid memory address";
            }
        } else if (regs.getAll().count(reg1_upper)) {  // Byte register destination
            std::set<std::string> byte_registers = {"AH", "AL", "BH", "BL", "CH", "CL", "DH", "DL"};
            if (byte_registers.count(reg1_upper) == 0) {
                status = "MOVB failed: Not a byte register";
            } else {
                std::string reg2_upper = reg2;
                std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
                if (reg2[0] == '[') {  // Memory to byte register
                    std::string mem_reg;
                    int32_t offset;
                    if (parseMemoryAddress(reg2, mem_reg, offset)) {
                        uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                        uint32_t mem_val = mem.read(addr, true);  // Read byte from memory
                        regs.set(reg1_upper, mem_val);
                        char debug_str[64];
                        snprintf(debug_str, sizeof(debug_str), "MOVB %s <- [%08X] = %02X", 
                                 reg1_upper.c_str(), addr, mem_val);
                        status = debug_str;
                        if (!is_running) {
                            history.push_back({cmd_addr, cmd});
                            regs.set("EIP", cmd_addr + 4);
                        }
                    } else {
                        status = "MOVB failed: Invalid memory address";
                    }
                } else {
                    status = "MOVB failed: Unsupported operand";
                }
            }
        } else {
            status = "MOVB failed: Invalid register";
        }
    } 
    // Arithmetic and comparison operations (ADD, XOR, SUB, CMP)
    else if (op_upper == "ADD" || op_upper == "XOR" || op_upper == "SUB" || op_upper == "CMP") {
        ss >> reg2;
        if (reg1[0] == '[') {  // Memory operand as destination/source
            std::string mem_reg;
            int32_t offset;
            if (parseMemoryAddress(reg1, mem_reg, offset)) {
                std::string reg2_upper = reg2;
                std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
                if (!regs.getAll().count(reg2_upper)) {
                    status = op_upper + " failed: Invalid register";
                } else {
                    uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                    uint32_t val1 = mem.read(addr);  // Value from memory
                    uint32_t val2 = regs.get(reg2_upper);  // Value from register
                    uint32_t result_val;
                    uint32_t flags = 0;

                    if (op_upper == "ADD") {  // Addition
                        int32_t signed_val1 = static_cast<int32_t>(val1);
                        int32_t signed_val2 = static_cast<int32_t>(val2);
                        int32_t signed_result = signed_val1 + signed_val2;
                        result_val = static_cast<uint32_t>(signed_result);
                        if (result_val == 0) flags |= ZF;  // Zero flag
                        if (signed_result < 0) flags |= SF;  // Sign flag
                        if (((signed_val1 > 0 && signed_val2 > 0 && signed_result < 0) ||
                             (signed_val1 < 0 && signed_val2 < 0 && signed_result > 0))) flags |= OF;  // Overflow flag
                        regs.set("FLAGS", flags);
                        mem.write(addr, result_val);
                        char debug_str[64];
                        snprintf(debug_str, sizeof(debug_str), "ADD [%08X]: %08X + %08X = %08X", 
                                 addr, val1, val2, result_val);
                        status = debug_str;
                    } else if (op_upper == "XOR") {  // Bitwise XOR
                        result_val = val1 ^ val2;
                        int32_t signed_result = static_cast<int32_t>(result_val);
                        if (result_val == 0) flags |= ZF;
                        if (signed_result < 0) flags |= SF;
                        regs.set("FLAGS", flags);
                        mem.write(addr, result_val);
                        char debug_str[64];
                        snprintf(debug_str, sizeof(debug_str), "XOR [%08X]: %08X ^ %08X = %08X", 
                                 addr, val1, val2, result_val);
                        status = debug_str;
                    } else if (op_upper == "SUB") {  // Subtraction
                        int32_t signed_val1 = static_cast<int32_t>(val1);
                        int32_t signed_val2 = static_cast<int32_t>(val2);
                        int32_t signed_result = signed_val1 - signed_val2;
                        result_val = static_cast<uint32_t>(signed_result);
                        if (result_val == 0) flags |= ZF;
                        if (signed_result < 0) flags |= SF;
                        if (((signed_val1 > 0 && signed_val2 < 0 && signed_result < 0) ||
                             (signed_val1 < 0 && signed_val2 > 0 && signed_result > 0))) flags |= OF;
                        regs.set("FLAGS", flags);
                        mem.write(addr, result_val);
                        char debug_str[64];
                        snprintf(debug_str, sizeof(debug_str), "SUB [%08X]: %08X - %08X = %08X", 
                                 addr, val1, val2, result_val);
                        status = debug_str;
                    } else { // CMP (Compare)
                        int32_t signed_val1 = static_cast<int32_t>(val1);
                        int32_t signed_val2 = static_cast<int32_t>(val2);
                        int32_t signed_result = signed_val1 - signed_val2;
                        result_val = static_cast<uint32_t>(signed_result);
                        uint32_t flags = regs.get("FLAGS"); //Preserve existing flags
                        if (signed_result == 0) flags |= ZF;
                        if (signed_result < 0) flags |= SF;
                        if (((signed_val1 > 0 && signed_val2 < 0 && signed_result < 0) ||
                             (signed_val1 < 0 && signed_val2 > 0 && signed_result > 0))) flags |= OF;
                        regs.set("FLAGS", flags);
                        std::stringstream ss;
                        ss << "CMP [" << std::hex << addr << "] - " << val2
                           << ": ZF=" << ((flags & CPU::ZF) != 0)
                           << " SF=" << ((flags & CPU::SF) != 0)
                           << " OF=" << ((flags & CPU::OF) != 0)
                           << " FLAGS=" << flags;
                        status = ss.str();
                    }
                    if (!is_running) {
                        history.push_back({cmd_addr, cmd});
                        regs.set("EIP", cmd_addr + 4);
                    }
                }
            } else {
                status = op_upper + " failed: Invalid memory address";
            }
        } else if (regs.getAll().count(reg1_upper)) {  // Register operand
            uint32_t val1 = regs.get(reg1_upper);
            uint32_t val2;
            uint32_t result_val;
            uint32_t flags = 0;

            std::string reg2_upper = reg2;
            std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
            if (regs.getAll().count(reg2_upper)) {  // Register operand
                val2 = regs.get(reg2_upper);
            } else {  // Immediate value
                try {
                    val2 = std::stoul(reg2_upper, nullptr, 16);
                } catch (...) {
                    status = op_upper + " failed: Invalid operand";
                    return status;
                }
            }

            if (op_upper == "ADD") {
                int32_t signed_val1 = static_cast<int32_t>(val1);
                int32_t signed_val2 = static_cast<int32_t>(val2);
                int32_t signed_result = signed_val1 + signed_val2;
                result_val = static_cast<uint32_t>(signed_result);
                if (result_val == 0) flags |= ZF;
                if (signed_result < 0) flags |= SF;
                if (((signed_val1 > 0 && signed_val2 > 0 && signed_result < 0) ||
                     (signed_val1 < 0 && signed_val2 < 0 && signed_result > 0))) flags |= OF;
                regs.set("FLAGS", flags);
                regs.set(reg1_upper, result_val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "ADD %s: %08X + %08X = %08X", 
                         reg1_upper.c_str(), val1, val2, result_val);
                status = debug_str;
            } else if (op_upper == "XOR") {
                result_val = val1 ^ val2;
                int32_t signed_result = static_cast<int32_t>(result_val);
                if (result_val == 0) flags |= ZF;
                if (signed_result < 0) flags |= SF;
                regs.set("FLAGS", flags);
                regs.set(reg1_upper, result_val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "XOR %s: %08X ^ %08X = %08X", 
                         reg1_upper.c_str(), val1, val2, result_val);
                status = debug_str;
            } else if (op_upper == "SUB") {
                int32_t signed_val1 = static_cast<int32_t>(val1);
                int32_t signed_val2 = static_cast<int32_t>(val2);
                int32_t signed_result = signed_val1 - signed_val2;
                result_val = static_cast<uint32_t>(signed_result);
                if (result_val == 0) flags |= ZF;
                if (signed_result < 0) flags |= SF;
                if (((signed_val1 > 0 && signed_val2 < 0 && signed_result < 0) ||
                     (signed_val1 < 0 && signed_val2 > 0 && signed_result > 0))) flags |= OF;
                regs.set("FLAGS", flags);
                regs.set(reg1_upper, result_val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "SUB %s: %08X - %08X = %08X", 
                         reg1_upper.c_str(), val1, val2, result_val);
                status = debug_str;
            } else { // CMP
                int32_t signed_val1 = static_cast<int32_t>(val1);
                int32_t signed_val2 = static_cast<int32_t>(val2);
                int32_t signed_result = signed_val1 - signed_val2;
                result_val = static_cast<uint32_t>(signed_result);
                if (signed_result == 0) flags |= ZF;
                if (signed_result < 0) flags |= SF;
                if (((signed_val1 > 0 && signed_val2 < 0 && signed_result < 0) ||
                     (signed_val1 < 0 && signed_val2 > 0 && signed_result > 0))) flags |= OF;
                regs.set("FLAGS", flags);
                uint32_t updated_flags = regs.get("FLAGS");
                std::stringstream ss;
                ss << "CMP " << reg1 << " - " << std::hex << val2
                   << ": ZF=" << ((flags & CPU::ZF) != 0)
                   << " SF=" << ((flags & CPU::SF) != 0)
                   << " OF=" << ((flags & CPU::OF) != 0)
                   << " FLAGS_set=" << flags
                   << " FLAGS_get=" << updated_flags;
                status = ss.str();
            }
            if (!is_running) {
                history.push_back({cmd_addr, cmd});
                regs.set("EIP", cmd_addr + 4);
            }
        } else {
            status = op_upper + " failed: Invalid register";
        }
    } 
    // PUSH: Push register value onto stack
    else if (op_upper == "PUSH") {
        if (regs.getAll().count(reg1_upper)) {
            uint32_t val = regs.get(reg1_upper);
            uint32_t esp = regs.get("ESP");
            char debug_str[128];
            snprintf(debug_str, sizeof(debug_str), "PUSH %s: ESP=%08X val=%08X STACK_BASE=%08X", 
                     reg1_upper.c_str(), esp, val, mem.STACK_BASE);
            status = debug_str;

            if (esp > mem.STACK_BASE) {  // Check stack bounds
                esp -= 4;  // Decrement stack pointer
                mem.write(esp, val);  // Write value to stack
                regs.set("ESP", esp);
                snprintf(debug_str, sizeof(debug_str), "Pushed %08X to %08X, new ESP=%08X", 
                         val, esp, regs.get("ESP"));
                status = debug_str;
                if (!is_running) {
                    history.push_back({cmd_addr, cmd});
                    regs.set("EIP", cmd_addr + 4);
                }
            } else {
                status = "PUSH failed: ESP <= STACK_BASE";
            }
        } else {
            status = "PUSH failed: Invalid register";
        }
    } 
    // POP: Pop value from stack into register
    else if (op_upper == "POP") {
        if (regs.getAll().count(reg1_upper)) {
            uint32_t esp = regs.get("ESP");
            if (esp <= mem.STACK_TOP - 4) {  // Check stack bounds
                uint32_t val = mem.read(esp);  // Read value from stack
                esp += 4;  // Increment stack pointer
                regs.set("ESP", esp);
                regs.set(reg1_upper, val);
                // Clear stack memory (4 bytes)
                mem.erase(esp - 4);
                mem.erase(esp - 3);
                mem.erase(esp - 2);
                mem.erase(esp - 1);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "POP %s: %08X from %08X, new ESP=%08X", 
                         reg1_upper.c_str(), val, esp - 4, esp);
                status = debug_str;
                if (!is_running) {
                    history.push_back({cmd_addr, cmd});
                    regs.set("EIP", cmd_addr + 4);
                }
            } else {
                status = "POP failed: Stack empty or overflow";
            }
        } else {
            status = "POP failed: Invalid register";
        }
    } 
    // JE/JZ: Jump if equal/zero
    else if (op_upper == "JE" || op_upper == "JZ") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);  // Parse target address
                uint32_t currentEIP = regs.get("EIP");
                uint32_t flags = regs.get("FLAGS");
                std::stringstream ss;
                ss << "JE: FLAGS=" << std::hex << flags 
                   << " ZF=" << CPU::ZF 
                   << " Cond=" << ((flags & CPU::ZF) != 0) 
                   << " EIP=" << currentEIP 
                   << " Target=" << target_addr;
                status = ss.str();

                if (flags & CPU::ZF) {  // Jump if zero flag is set
                    regs.set("EIP", target_addr);
                    status += " Jumped to " + reg1;
                } else {
                    status += " No jump";
                    uint32_t instructionSize = 6;  // Example size, should be calculated based on instruction
                    regs.set("EIP", currentEIP + instructionSize);
                }
                if (!is_running) history.push_back({cmd_addr, cmd});
            } catch (...) {
                status = "JE/JZ failed: Invalid address";
            }
        } else {
            status = "JE/JZ failed: Missing address";
        }
    } 
    // JNE/JNZ: Jump if not equal/not zero
    else if (op_upper == "JNE" || op_upper == "JNZ") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                if (!(regs.get("FLAGS") & ZF)) {  // Jump if zero flag is not set
                    regs.set("EIP", target_addr);
                    status = "JNE/JNZ jumped to " + reg1;
                } else {
                    status = "JNE/JNZ no jump";
                    if (!is_running) regs.set("EIP", cmd_addr + 4);
                }
                if (!is_running) history.push_back({cmd_addr, cmd});
            } catch (...) {
                status = "JNE/JNZ failed: Invalid address";
            }
        } else {
            status = "JNE/JNZ failed: Missing address";
        }
    } 
    // JG: Jump if greater
    else if (op_upper == "JG") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                bool zf = regs.get("FLAGS") & ZF;
                bool sf = regs.get("FLAGS") & SF;
                bool of = regs.get("FLAGS") & OF;
                if (!zf && (sf == of)) {  // Jump if not zero and sign matches overflow
                    regs.set("EIP", target_addr);
                    status = "JG jumped to " + reg1;
                } else {
                    status = "JG no jump";
                    if (!is_running) regs.set("EIP", cmd_addr + 4);
                }
                if (!is_running) history.push_back({cmd_addr, cmd});
            } catch (...) {
                status = "JG failed: Invalid address";
            }
        } else {
            status = "JG failed: Missing address";
        }
    } 
    // JL: Jump if less
    else if (op_upper == "JL") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                bool sf = regs.get("FLAGS") & SF;
                bool of = regs.get("FLAGS") & OF;
                if (sf != of) {  // Jump if sign differs from overflow
                    regs.set("EIP", target_addr);
                    status = "JL jumped to " + reg1;
                } else {
                    status = "JL no jump";
                    if (!is_running) regs.set("EIP", cmd_addr + 4);
                }
                if (!is_running) history.push_back({cmd_addr, cmd});
            } catch (...) {
                status = "JL failed: Invalid address";
            }
        } else {
            status = "JL failed: Missing address";
        }
    } 
    // JGE: Jump if greater or equal
    else if (op_upper == "JGE") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                bool sf = regs.get("FLAGS") & SF;
                bool of = regs.get("FLAGS") & OF;
                if (sf == of) {  // Jump if sign matches overflow
                    regs.set("EIP", target_addr);
                    status = "JGE jumped to " + reg1;
                } else {
                    status = "JGE no jump";
                    if (!is_running) regs.set("EIP", cmd_addr + 4);
                }
                if (!is_running) history.push_back({cmd_addr, cmd});
            } catch (...) {
                status = "JGE failed: Invalid address";
            }
        } else {
            status = "JGE failed: Missing address";
        }
    } 
    // JLE: Jump if less or equal
    else if (op_upper == "JLE") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                bool zf = regs.get("FLAGS") & ZF;
                bool sf = regs.get("FLAGS") & SF;
                bool of = regs.get("FLAGS") & OF;
                if (zf || (sf != of)) {  // Jump if zero or sign differs from overflow
                    regs.set("EIP", target_addr);
                    status = "JLE jumped to " + reg1;
                } else {
                    status = "JLE no jump";
                    if (!is_running) regs.set("EIP", cmd_addr + 4);
                }
                if (!is_running) history.push_back({cmd_addr, cmd});
            } catch (...) {
                status = "JLE failed: Invalid address";
            }
        } else {
            status = "JLE failed: Missing address";
        }
    } 
    // RUN: Execute all commands in history
    else if (op_upper == "RUN") {
        if (!history.empty()) {
            is_running = true;  // Set running flag
            regs.set("EIP", PROGRAM_BASE);  // Reset EIP to program start
            uint32_t program_end = PROGRAM_BASE + (history.size() * 4);  // Calculate program end
            while (regs.get("EIP") >= PROGRAM_BASE && regs.get("EIP") < program_end) {
                size_t index = (regs.get("EIP") - PROGRAM_BASE) / 4;  // Calculate history index
                if (index >= history.size()) break;
                std::string current_cmd = history[index].second;  // Get command from history
                status = execute(current_cmd, memory_start_addr);  // Recursively execute command
                if (status == "QUIT") {
                    is_running = false;
                    return status;
                }
                std::string op_check;
                std::stringstream ss_check(current_cmd);
                ss_check >> op_check;
                std::transform(op_check.begin(), op_check.end(), op_check.begin(), ::toupper);
                // Increment EIP for non-jump instructions
                if (op_check != "JE" && op_check != "JZ" && op_check != "JNE" && op_check != "JNZ" &&
                    op_check != "JG" && op_check != "JL" && op_check != "JGE" && op_check != "JLE") {
                    regs.set("EIP", regs.get("EIP") + 4);
                }
                usleep(1000000);  // 1s delay for visibility
            }
            is_running = false;
            status = "RUN completed";
            if (!history.empty()) {
                history.push_back({cmd_addr, cmd});  // Record RUN command
            }
        } else {
            status = "RUN failed: No history";
        }
    } 
    // CLEAR: Reset registers, stack, or history
    else if (op_upper == "CLEAR") {
        std::string mode = reg1;
        if (mode.empty()) mode = "ALL";  // Default to ALL if no mode specified
        std::transform(mode.begin(), mode.end(), mode.begin(), ::toupper);
        if (mode == "ALL" || mode == "REGS") {  // Clear registers
            for (auto& reg : regs.getAll()) {
                if (reg.first == "ESP" && mode != "REGS") regs.set(reg.first, mem.STACK_TOP);
                else if (reg.first == "SP" && mode != "REGS") regs.set(reg.first, mem.STACK_TOP & 0xFFFF);
                else regs.set(reg.first, 0);
            }
            if (mode == "ALL") regs.set("EIP", PROGRAM_BASE);  // Reset EIP for ALL
        }
        if (mode == "ALL" || mode == "STACK") {  // Clear stack
            mem.clear();
            regs.set("ESP", mem.STACK_TOP);
            regs.set("SP", mem.STACK_TOP & 0xFFFF);
        }
        if (mode == "ALL" || mode == "HISTORY") {  // Clear history
            clearHistory();
        }
        status = "CLEAR " + mode + " executed";
        if (!is_running && mode != "ALL") {  // Don’t log CLEAR ALL
            history.push_back({cmd_addr, cmd});
            regs.set("EIP", cmd_addr + 4);
        }
    } 
    // MEMVIEW: View memory at specified address
    else if (op_upper == "MEMVIEW") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t addr = std::stoul(addr_upper, nullptr, 16);
                return memview(addr, reg1, memory_start_addr);  // Call memview function
            } catch (...) {
                status = "MEMVIEW failed: Invalid address";
            }
        } else {
            status = "MEMVIEW failed: Missing address";
        }
    } 
    // SETTEXT: Write text to memory
    else if (op_upper == "SETTEXT") {
        if (!reg1.empty() && cmd.find('"') != std::string::npos) {
            try {
                std::string addr_str = reg1;
                std::transform(addr_str.begin(), addr_str.end(), addr_str.begin(), ::toupper);
                uint32_t addr = std::stoul(addr_str, nullptr, 16);
                size_t quote_start = cmd.find('"');
                size_t quote_end = cmd.find('"', quote_start + 1);
                if (quote_start != std::string::npos && quote_end != std::string::npos && quote_end > quote_start + 1) {
                    std::string text = cmd.substr(quote_start + 1, quote_end - quote_start - 1);
                    mem.writeText(addr, text);  // Write text to memory
                    char debug_str[128];
                    snprintf(debug_str, sizeof(debug_str), 
                             "SETTEXT: Wrote \"%s\" at %s: [%02x %02x %02x %02x %02x %02x]", 
                             text.c_str(), reg1.c_str(),
                             mem.read(addr, true), mem.read(addr + 1, true), mem.read(addr + 2, true),
                             mem.read(addr + 3, true), mem.read(addr + 4, true), mem.read(addr + 5, true));
                    status = debug_str;
                    if (!is_running) {
                        history.push_back({cmd_addr, cmd});
                        regs.set("EIP", cmd_addr + 4);
                    }
                } else {
                    status = "SETTEXT failed: Invalid text format";
                }
            } catch (...) {
                status = "SETTEXT failed: Invalid address";
            }
        } else {
            status = "SETTEXT failed: Missing address or text";
        }
    } 
    // HELP: Display help message
    else if (op_upper == "HELP") {
        status = "HELP: See emulator for full command list";
        if (!is_running) {
            history.push_back({cmd_addr, cmd});
            regs.set("EIP", cmd_addr + 4);
        }
    } 
    // QUIT: Exit emulator
    else if (op_upper == "QUIT") {
        if (!is_running) {
            history.push_back({cmd_addr, cmd});
            regs.set("EIP", cmd_addr + 4);
        }
        status = "QUIT";
    } 
    // Unknown command
    else {
        status = "Unknown command: " + cmd;
    }

    return status;
}

// Returns reference to command history
std::vector<std::pair<uint32_t, std::string>>& CPU::getHistory() {
    return history;  // Return vector of address-command pairs
}

// Clears command history
void CPU::clearHistory() {
    history.clear();  // Empty the history vector
}

// Parses memory address in format [reg+offset] or [address]
bool CPU::parseMemoryAddress(const std::string& arg, std::string& reg_out, int32_t& offset_out) {
    if (arg.size() < 3 || arg[0] != '[' || arg.back() != ']') return false;  // Validate syntax
    std::string inner = arg.substr(1, arg.size() - 2);  // Extract content inside brackets
    size_t plus_pos = inner.find('+');
    size_t minus_pos = inner.find('-');
    std::string reg_str, offset_str;

    if (plus_pos != std::string::npos) {  // [reg+offset]
        reg_str = inner.substr(0, plus_pos);
        offset_str = inner.substr(plus_pos + 1);
        offset_out = std::stoi(offset_str, nullptr, 16);  // Positive offset
    } else if (minus_pos != std::string::npos) {  // [reg-offset]
        reg_str = inner.substr(0, minus_pos);
        offset_str = inner.substr(minus_pos + 1);
        offset_out = -std::stoi(offset_str, nullptr, 16);  // Negative offset
    } else {  // [reg] or [address]
        reg_str = inner;
        offset_out = 0;
    }

    std::transform(reg_str.begin(), reg_str.end(), reg_str.begin(), ::toupper);
    if (regs.getAll().count(reg_str)) {  // Valid register
        reg_out = reg_str;
        return true;
    } else {  // Direct address
        try {
            uint32_t direct_addr = std::stoul(reg_str, nullptr, 16);
            reg_out = "";
            offset_out = direct_addr;
            return true;
        } catch (...) {
            return false;
        }
    }
}

// Placeholder for running history (implemented in RUN command)
void CPU::runHistory() {
    // Implemented within RUN command
}
