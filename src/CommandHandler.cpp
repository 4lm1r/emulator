#include "CommandHandler.hpp"
#include "CPU.hpp"
#include <sstream>
#include <algorithm>
#include <set>
#include <cstring>
#include <unistd.h> // For usleep in cmdRun

CommandHandler::CommandHandler(CPU& cpu_ref) : cpu(cpu_ref), regs(cpu_ref.regs), mem(cpu_ref.mem) {
    // Initialize command map
    commandMap["MOV"] = [this](const std::string& cmd, uint32_t* addr) { return cmdMov(cmd, addr); };
    commandMap["MOVB"] = [this](const std::string& cmd, uint32_t* addr) { return cmdMovb(cmd, addr); };
    commandMap["ADD"] = [this](const std::string& cmd, uint32_t* addr) { return cmdAdd(cmd, addr); };
    commandMap["XOR"] = [this](const std::string& cmd, uint32_t* addr) { return cmdXor(cmd, addr); };
    commandMap["SUB"] = [this](const std::string& cmd, uint32_t* addr) { return cmdSub(cmd, addr); };
    commandMap["CMP"] = [this](const std::string& cmd, uint32_t* addr) { return cmdCmp(cmd, addr); };
    commandMap["PUSH"] = [this](const std::string& cmd, uint32_t* addr) { return cmdPush(cmd, addr); };
    commandMap["POP"] = [this](const std::string& cmd, uint32_t* addr) { return cmdPop(cmd, addr); };
    commandMap["JE"] = [this](const std::string& cmd, uint32_t* addr) { return cmdJe(cmd, addr); };
    commandMap["JZ"] = [this](const std::string& cmd, uint32_t* addr) { return cmdJe(cmd, addr); }; // Alias for JE
    commandMap["JNE"] = [this](const std::string& cmd, uint32_t* addr) { return cmdJne(cmd, addr); };
    commandMap["JNZ"] = [this](const std::string& cmd, uint32_t* addr) { return cmdJne(cmd, addr); }; // Alias for JNE
    commandMap["JG"] = [this](const std::string& cmd, uint32_t* addr) { return cmdJg(cmd, addr); };
    commandMap["JL"] = [this](const std::string& cmd, uint32_t* addr) { return cmdJl(cmd, addr); };
    commandMap["JGE"] = [this](const std::string& cmd, uint32_t* addr) { return cmdJge(cmd, addr); };
    commandMap["JLE"] = [this](const std::string& cmd, uint32_t* addr) { return cmdJle(cmd, addr); };
    commandMap["RUN"] = [this](const std::string& cmd, uint32_t* addr) { return cmdRun(cmd, addr); };
    commandMap["CLEAR"] = [this](const std::string& cmd, uint32_t* addr) { return cmdClear(cmd, addr); };
    commandMap["MEMSET"] = [this](const std::string& cmd, uint32_t* addr) { return cmdMemset(cmd, addr); };
    commandMap["SETTEXT"] = [this](const std::string& cmd, uint32_t* addr) { return cmdSettext(cmd, addr); };
    commandMap["MEMVIEW"] = [this](const std::string& cmd, uint32_t* addr) { return cmdMemview(cmd, addr); };
    commandMap["HELP"] = [this](const std::string& cmd, uint32_t* addr) { return cmdHelp(cmd, addr); };
    commandMap["QUIT"] = [this](const std::string& cmd, uint32_t* addr) { return cmdQuit(cmd, addr); };
}

std::string CommandHandler::executeCommand(const std::string& cmd, uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op;
    ss >> op;
    std::string op_upper = op;
    std::transform(op_upper.begin(), op_upper.end(), op_upper.begin(), ::toupper);

    auto it = commandMap.find(op_upper);
    if (it != commandMap.end()) {
        return it->second(cmd, memory_start_addr);
    }
    return "Unknown command: " + cmd;
}

// Command implementations
std::string CommandHandler::cmdMov(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1, reg2;
    ss >> op >> reg1 >> reg2;
    std::string reg1_upper = reg1, reg2_upper = reg2;
    std::transform(reg1_upper.begin(), reg1_upper.end(), reg1_upper.begin(), ::toupper);
    std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (reg1[0] == '[') {  // Memory destination
        std::string mem_reg;
        int32_t offset;
        if (parseMemoryAddress(reg1, mem_reg, offset)) {
            uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
            uint32_t val;
            if (regs.getAll().count(reg2_upper)) {  // Register to memory
                val = regs.get(reg2_upper);
                mem.write(addr, val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "MOV [%08X] <- %08X", addr, val);
                status = debug_str;
            } else {  // Immediate to memory
                try {
                    val = std::stoul(reg2_upper, nullptr, 16);
                    mem.write(addr, val);
                    char debug_str[64];
                    snprintf(debug_str, sizeof(debug_str), "MOV [%08X] <- %08X", addr, val);
                    status = debug_str;
                } catch (...) {
                    status = "MOV failed: Invalid value";
                }
            }
        } else {
            status = "MOV failed: Invalid memory address";
        }
    } else if (regs.getAll().count(reg1_upper)) {  // Register destination
        if (regs.getAll().count(reg2_upper)) {  // Register to register
            uint32_t val = regs.get(reg2_upper);
            regs.set(reg1_upper, val);
            char debug_str[64];
            snprintf(debug_str, sizeof(debug_str), "MOV %s <- %08X", reg1_upper.c_str(), val);
            status = debug_str;
        } else if (reg2[0] == '[') {  // Memory to register
            std::string mem_reg;
            int32_t offset;
            if (parseMemoryAddress(reg2, mem_reg, offset)) {
                uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                uint32_t mem_val = mem.read(addr);
                regs.set(reg1_upper, mem_val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "MOV %s <- [%08X] = %08X", reg1_upper.c_str(), addr, mem_val);
                status = debug_str;
            } else {
                status = "MOV failed: Invalid memory address";
            }
        } else {  // Immediate to register
            try {
                uint32_t val = std::stoul(reg2_upper, nullptr, 16);
                regs.set(reg1_upper, val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "MOV %s <- %08X", reg1_upper.c_str(), val);
                status = debug_str;
            } catch (...) {
                status = "MOV failed: Invalid value";
            }
        }
    } else {
        status = "MOV failed: Invalid register";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdMovb(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1, reg2;
    ss >> op >> reg1 >> reg2;
    std::string reg1_upper = reg1, reg2_upper = reg2;
    std::transform(reg1_upper.begin(), reg1_upper.end(), reg1_upper.begin(), ::toupper);
    std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (reg1[0] == '[') {  // Memory destination
        std::string mem_reg;
        int32_t offset;
        if (parseMemoryAddress(reg1, mem_reg, offset)) {
            uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
            try {
                uint32_t val = std::stoul(reg2_upper, nullptr, 16);
                if (val > 0xFF) {
                    status = "MOVB failed: Value exceeds byte size";
                } else {
                    mem.write(addr, val, true);
                    char debug_str[64];
                    snprintf(debug_str, sizeof(debug_str), "MOVB [%08X] <- %02X", addr, val);
                    status = debug_str;
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
        } else if (reg2[0] == '[') {  // Memory to byte register
            std::string mem_reg;
            int32_t offset;
            if (parseMemoryAddress(reg2, mem_reg, offset)) {
                uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                uint32_t mem_val = mem.read(addr, true);
                regs.set(reg1_upper, mem_val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "MOVB %s <- [%08X] = %02X", reg1_upper.c_str(), addr, mem_val);
                status = debug_str;
            } else {
                status = "MOVB failed: Invalid memory address";
            }
        } else {
            status = "MOVB failed: Unsupported operand";
        }
    } else {
        status = "MOVB failed: Invalid register";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdAdd(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1, reg2;
    ss >> op >> reg1 >> reg2;
    std::string reg1_upper = reg1, reg2_upper = reg2;
    std::transform(reg1_upper.begin(), reg1_upper.end(), reg1_upper.begin(), ::toupper);
    std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (reg1[0] == '[') {  // Memory operand
        std::string mem_reg;
        int32_t offset;
        if (parseMemoryAddress(reg1, mem_reg, offset)) {
            if (!regs.getAll().count(reg2_upper)) {
                status = "ADD failed: Invalid register";
            } else {
                uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                uint32_t val1 = mem.read(addr);
                uint32_t val2 = regs.get(reg2_upper);
                int32_t signed_val1 = static_cast<int32_t>(val1);
                int32_t signed_val2 = static_cast<int32_t>(val2);
                int32_t signed_result = signed_val1 + signed_val2;
                uint32_t result_val = static_cast<uint32_t>(signed_result);
                uint32_t flags = 0;
                if (result_val == 0) flags |= CPU::ZF;
                if (signed_result < 0) flags |= CPU::SF;
                if (((signed_val1 > 0 && signed_val2 > 0 && signed_result < 0) ||
                     (signed_val1 < 0 && signed_val2 < 0 && signed_result > 0))) flags |= CPU::OF;
                regs.set("FLAGS", flags);
                mem.write(addr, result_val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "ADD [%08X]: %08X + %08X = %08X", addr, val1, val2, result_val);
                status = debug_str;
            }
        } else {
            status = "ADD failed: Invalid memory address";
        }
    } else if (regs.getAll().count(reg1_upper)) {  // Register operand
        uint32_t val1 = regs.get(reg1_upper);
        uint32_t val2;
        if (regs.getAll().count(reg2_upper)) {
            val2 = regs.get(reg2_upper);
        } else {
            try {
                val2 = std::stoul(reg2_upper, nullptr, 16);
            } catch (...) {
                status = "ADD failed: Invalid operand";
                return status;
            }
        }
        int32_t signed_val1 = static_cast<int32_t>(val1);
        int32_t signed_val2 = static_cast<int32_t>(val2);
        int32_t signed_result = signed_val1 + signed_val2;
        uint32_t result_val = static_cast<uint32_t>(signed_result);
        uint32_t flags = 0;
        if (result_val == 0) flags |= CPU::ZF;
        if (signed_result < 0) flags |= CPU::SF;
        if (((signed_val1 > 0 && signed_val2 > 0 && signed_result < 0) ||
             (signed_val1 < 0 && signed_val2 < 0 && signed_result > 0))) flags |= CPU::OF;
        regs.set("FLAGS", flags);
        regs.set(reg1_upper, result_val);
        char debug_str[64];
        snprintf(debug_str, sizeof(debug_str), "ADD %s: %08X + %08X = %08X", reg1_upper.c_str(), val1, val2, result_val);
        status = debug_str;
    } else {
        status = "ADD failed: Invalid register";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdXor(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1, reg2;
    ss >> op >> reg1 >> reg2;
    std::string reg1_upper = reg1, reg2_upper = reg2;
    std::transform(reg1_upper.begin(), reg1_upper.end(), reg1_upper.begin(), ::toupper);
    std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (reg1[0] == '[') {
        std::string mem_reg;
        int32_t offset;
        if (parseMemoryAddress(reg1, mem_reg, offset)) {
            if (!regs.getAll().count(reg2_upper)) {
                status = "XOR failed: Invalid register";
            } else {
                uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                uint32_t val1 = mem.read(addr);
                uint32_t val2 = regs.get(reg2_upper);
                uint32_t result_val = val1 ^ val2;
                int32_t signed_result = static_cast<int32_t>(result_val);
                uint32_t flags = 0;
                if (result_val == 0) flags |= CPU::ZF;
                if (signed_result < 0) flags |= CPU::SF;
                regs.set("FLAGS", flags);
                mem.write(addr, result_val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "XOR [%08X]: %08X ^ %08X = %08X", addr, val1, val2, result_val);
                status = debug_str;
            }
        } else {
            status = "XOR failed: Invalid memory address";
        }
    } else if (regs.getAll().count(reg1_upper)) {
        uint32_t val1 = regs.get(reg1_upper);
        uint32_t val2;
        if (regs.getAll().count(reg2_upper)) {
            val2 = regs.get(reg2_upper);
        } else {
            try {
                val2 = std::stoul(reg2_upper, nullptr, 16);
            } catch (...) {
                status = "XOR failed: Invalid operand";
                return status;
            }
        }
        uint32_t result_val = val1 ^ val2;
        int32_t signed_result = static_cast<int32_t>(result_val);
        uint32_t flags = 0;
        if (result_val == 0) flags |= CPU::ZF;
        if (signed_result < 0) flags |= CPU::SF;
        regs.set("FLAGS", flags);
        regs.set(reg1_upper, result_val);
        char debug_str[64];
        snprintf(debug_str, sizeof(debug_str), "XOR %s: %08X ^ %08X = %08X", reg1_upper.c_str(), val1, val2, result_val);
        status = debug_str;
    } else {
        status = "XOR failed: Invalid register";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdSub(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1, reg2;
    ss >> op >> reg1 >> reg2;
    std::string reg1_upper = reg1, reg2_upper = reg2;
    std::transform(reg1_upper.begin(), reg1_upper.end(), reg1_upper.begin(), ::toupper);
    std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (reg1[0] == '[') {
        std::string mem_reg;
        int32_t offset;
        if (parseMemoryAddress(reg1, mem_reg, offset)) {
            if (!regs.getAll().count(reg2_upper)) {
                status = "SUB failed: Invalid register";
            } else {
                uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                uint32_t val1 = mem.read(addr);
                uint32_t val2 = regs.get(reg2_upper);
                int32_t signed_val1 = static_cast<int32_t>(val1);
                int32_t signed_val2 = static_cast<int32_t>(val2);
                int32_t signed_result = signed_val1 - signed_val2;
                uint32_t result_val = static_cast<uint32_t>(signed_result);
                uint32_t flags = 0;
                if (result_val == 0) flags |= CPU::ZF;
                if (signed_result < 0) flags |= CPU::SF;
                if (((signed_val1 > 0 && signed_val2 < 0 && signed_result < 0) ||
                     (signed_val1 < 0 && signed_val2 > 0 && signed_result > 0))) flags |= CPU::OF;
                regs.set("FLAGS", flags);
                mem.write(addr, result_val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "SUB [%08X]: %08X - %08X = %08X", addr, val1, val2, result_val);
                status = debug_str;
            }
        } else {
            status = "SUB failed: Invalid memory address";
        }
    } else if (regs.getAll().count(reg1_upper)) {
        uint32_t val1 = regs.get(reg1_upper);
        uint32_t val2;
        if (regs.getAll().count(reg2_upper)) {
            val2 = regs.get(reg2_upper);
        } else {
            try {
                val2 = std::stoul(reg2_upper, nullptr, 16);
            } catch (...) {
                status = "SUB failed: Invalid operand";
                return status;
            }
        }
        int32_t signed_val1 = static_cast<int32_t>(val1);
        int32_t signed_val2 = static_cast<int32_t>(val2);
        int32_t signed_result = signed_val1 - signed_val2;
        uint32_t result_val = static_cast<uint32_t>(signed_result);
        uint32_t flags = 0;
        if (result_val == 0) flags |= CPU::ZF;
        if (signed_result < 0) flags |= CPU::SF;
        if (((signed_val1 > 0 && signed_val2 < 0 && signed_result < 0) ||
             (signed_val1 < 0 && signed_val2 > 0 && signed_result > 0))) flags |= CPU::OF;
        regs.set("FLAGS", flags);
        regs.set(reg1_upper, result_val);
        char debug_str[64];
        snprintf(debug_str, sizeof(debug_str), "SUB %s: %08X - %08X = %08X", reg1_upper.c_str(), val1, val2, result_val);
        status = debug_str;
    } else {
        status = "SUB failed: Invalid register";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdCmp(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1, reg2;
    ss >> op >> reg1 >> reg2;
    std::string reg1_upper = reg1, reg2_upper = reg2;
    std::transform(reg1_upper.begin(), reg1_upper.end(), reg1_upper.begin(), ::toupper);
    std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (reg1[0] == '[') {
        std::string mem_reg;
        int32_t offset;
        if (parseMemoryAddress(reg1, mem_reg, offset)) {
            if (!regs.getAll().count(reg2_upper)) {
                status = "CMP failed: Invalid register";
            } else {
                uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                uint32_t val1 = mem.read(addr);
                uint32_t val2 = regs.get(reg2_upper);
                int32_t signed_val1 = static_cast<int32_t>(val1);
                int32_t signed_val2 = static_cast<int32_t>(val2);
                int32_t signed_result = signed_val1 - signed_val2;
                uint32_t flags = 0;
                if (signed_result == 0) flags |= CPU::ZF;
                if (signed_result < 0) flags |= CPU::SF;
                if (((signed_val1 > 0 && signed_val2 < 0 && signed_result < 0) ||
                     (signed_val1 < 0 && signed_val2 > 0 && signed_result > 0))) flags |= CPU::OF;
                regs.set("FLAGS", flags);
                std::stringstream ss;
                ss << "CMP [" << std::hex << addr << "] - " << val2
                   << ": ZF=" << ((flags & CPU::ZF) != 0)
                   << " SF=" << ((flags & CPU::SF) != 0)
                   << " OF=" << ((flags & CPU::OF) != 0)
                   << " FLAGS=" << flags;
                status = ss.str();
            }
        } else {
            status = "CMP failed: Invalid memory address";
        }
    } else if (regs.getAll().count(reg1_upper)) {
        uint32_t val1 = regs.get(reg1_upper);
        uint32_t val2;
        if (regs.getAll().count(reg2_upper)) {
            val2 = regs.get(reg2_upper);
        } else {
            try {
                val2 = std::stoul(reg2_upper, nullptr, 16);
            } catch (...) {
                status = "CMP failed: Invalid operand";
                return status;
            }
        }
        int32_t signed_val1 = static_cast<int32_t>(val1);
        int32_t signed_val2 = static_cast<int32_t>(val2);
        int32_t signed_result = signed_val1 - signed_val2;
        uint32_t flags = 0;
        if (signed_result == 0) flags |= CPU::ZF;
        if (signed_result < 0) flags |= CPU::SF;
        if (((signed_val1 > 0 && signed_val2 < 0 && signed_result < 0) ||
             (signed_val1 < 0 && signed_val2 > 0 && signed_result > 0))) flags |= CPU::OF;
        regs.set("FLAGS", flags);
        std::stringstream ss;
        ss << "CMP " << reg1 << " - " << std::hex << val2
           << ": ZF=" << ((flags & CPU::ZF) != 0)
           << " SF=" << ((flags & CPU::SF) != 0)
           << " OF=" << ((flags & CPU::OF) != 0)
           << " FLAGS=" << flags;
        status = ss.str();
    } else {
        status = "CMP failed: Invalid register";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdPush(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1;
    ss >> op >> reg1;
    std::string reg1_upper = reg1;
    std::transform(reg1_upper.begin(), reg1_upper.end(), reg1_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (regs.getAll().count(reg1_upper)) {
        uint32_t val = regs.get(reg1_upper);
        uint32_t esp = regs.get("ESP");
        if (esp > mem.STACK_BASE) {
            esp -= 4;
            mem.write(esp, val);
            regs.set("ESP", esp);
            char debug_str[64];
            snprintf(debug_str, sizeof(debug_str), "Pushed %08X to %08X, new ESP=%08X", val, esp, regs.get("ESP"));
            status = debug_str;
        } else {
            status = "PUSH failed: ESP <= STACK_BASE";
        }
    } else {
        status = "PUSH failed: Invalid register";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdPop(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1;
    ss >> op >> reg1;
    std::string reg1_upper = reg1;
    std::transform(reg1_upper.begin(), reg1_upper.end(), reg1_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (regs.getAll().count(reg1_upper)) {
        uint32_t esp = regs.get("ESP");
        if (esp <= mem.STACK_TOP - 4) {
            uint32_t val = mem.read(esp);
            esp += 4;
            regs.set("ESP", esp);
            regs.set(reg1_upper, val);
            mem.erase(esp - 4);
            mem.erase(esp - 3);
            mem.erase(esp - 2);
            mem.erase(esp - 1);
            char debug_str[64];
            snprintf(debug_str, sizeof(debug_str), "POP %s: %08X from %08X, new ESP=%08X", reg1_upper.c_str(), val, esp - 4, esp);
            status = debug_str;
        } else {
            status = "POP failed: Stack empty or overflow";
        }
    } else {
        status = "POP failed: Invalid register";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdJe(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1;
    ss >> op >> reg1;
    std::string addr_upper = reg1;
    std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (!reg1.empty()) {
        try {
            uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
            uint32_t flags = regs.get("FLAGS");
            std::stringstream ss;
            ss << "JE: FLAGS=" << std::hex << flags
               << " ZF=" << CPU::ZF
               << " Cond=" << ((flags & CPU::ZF) != 0)
               << " EIP=" << cmd_addr
               << " Target=" << target_addr;
            status = ss.str();

            if (flags & CPU::ZF) {
                regs.set("EIP", target_addr);
                status += " Jumped to " + reg1;
            } else {
                status += " No jump";
                regs.set("EIP", cmd_addr + 6); // Assuming 6-byte instruction
            }
        } catch (...) {
            status = "JE failed: Invalid address";
        }
    } else {
        status = "JE failed: Missing address";
    }

    if (!cpu.is_running) cpu.history.push_back({cmd_addr, cmd});
    return status;
}

std::string CommandHandler::cmdJne(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1;
    ss >> op >> reg1;
    std::string addr_upper = reg1;
    std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (!reg1.empty()) {
        try {
            uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
            if (!(regs.get("FLAGS") & CPU::ZF)) {
                regs.set("EIP", target_addr);
                status = "JNE jumped to " + reg1;
            } else {
                status = "JNE no jump";
                if (!cpu.is_running) regs.set("EIP", cmd_addr + 4);
            }
        } catch (...) {
            status = "JNE failed: Invalid address";
        }
    } else {
        status = "JNE failed: Missing address";
    }

    if (!cpu.is_running) cpu.history.push_back({cmd_addr, cmd});
    return status;
}

std::string CommandHandler::cmdJg(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1;
    ss >> op >> reg1;
    std::string addr_upper = reg1;
    std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (!reg1.empty()) {
        try {
            uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
            bool zf = regs.get("FLAGS") & CPU::ZF;
            bool sf = regs.get("FLAGS") & CPU::SF;
            bool of = regs.get("FLAGS") & CPU::OF;
            if (!zf && (sf == of)) {
                regs.set("EIP", target_addr);
                status = "JG jumped to " + reg1;
            } else {
                status = "JG no jump";
                if (!cpu.is_running) regs.set("EIP", cmd_addr + 4);
            }
        } catch (...) {
            status = "JG failed: Invalid address";
        }
    } else {
        status = "JG failed: Missing address";
    }

    if (!cpu.is_running) cpu.history.push_back({cmd_addr, cmd});
    return status;
}

std::string CommandHandler::cmdJl(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1;
    ss >> op >> reg1;
    std::string addr_upper = reg1;
    std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (!reg1.empty()) {
        try {
            uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
            bool sf = regs.get("FLAGS") & CPU::SF;
            bool of = regs.get("FLAGS") & CPU::OF;
            if (sf != of) {
                regs.set("EIP", target_addr);
                status = "JL jumped to " + reg1;
            } else {
                status = "JL no jump";
                if (!cpu.is_running) regs.set("EIP", cmd_addr + 4);
            }
        } catch (...) {
            status = "JL failed: Invalid address";
        }
    } else {
        status = "JL failed: Missing address";
    }

    if (!cpu.is_running) cpu.history.push_back({cmd_addr, cmd});
    return status;
}

std::string CommandHandler::cmdJge(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1;
    ss >> op >> reg1;
    std::string addr_upper = reg1;
    std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (!reg1.empty()) {
        try {
            uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
            bool sf = regs.get("FLAGS") & CPU::SF;
            bool of = regs.get("FLAGS") & CPU::OF;
            if (sf == of) {
                regs.set("EIP", target_addr);
                status = "JGE jumped to " + reg1;
            } else {
                status = "JGE no jump";
                if (!cpu.is_running) regs.set("EIP", cmd_addr + 4);
            }
        } catch (...) {
            status = "JGE failed: Invalid address";
        }
    } else {
        status = "JGE failed: Missing address";
    }

    if (!cpu.is_running) cpu.history.push_back({cmd_addr, cmd});
    return status;
}

std::string CommandHandler::cmdJle(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, reg1;
    ss >> op >> reg1;
    std::string addr_upper = reg1;
    std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (!reg1.empty()) {
        try {
            uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
            bool zf = regs.get("FLAGS") & CPU::ZF;
            bool sf = regs.get("FLAGS") & CPU::SF;
            bool of = regs.get("FLAGS") & CPU::OF;
            if (zf || (sf != of)) {
                regs.set("EIP", target_addr);
                status = "JLE jumped to " + reg1;
            } else {
                status = "JLE no jump";
                if (!cpu.is_running) regs.set("EIP", cmd_addr + 4);
            }
        } catch (...) {
            status = "JLE failed: Invalid address";
        }
    } else {
        status = "JLE failed: Missing address";
    }

    if (!cpu.is_running) cpu.history.push_back({cmd_addr, cmd});
    return status;
}

std::string CommandHandler::cmdRun(const std::string& cmd, uint32_t* memory_start_addr) {
    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (!cpu.history.empty()) {
        cpu.is_running = true;
        regs.set("EIP", CPU::PROGRAM_BASE);
        uint32_t program_end = CPU::PROGRAM_BASE + (cpu.history.size() * 4);
        while (regs.get("EIP") >= CPU::PROGRAM_BASE && regs.get("EIP") < program_end) {
            size_t index = (regs.get("EIP") - CPU::PROGRAM_BASE) / 4;
            if (index >= cpu.history.size()) break;
            std::string current_cmd = cpu.history[index].second;
            status = executeCommand(current_cmd, memory_start_addr);
            if (status == "QUIT") {
                cpu.is_running = false;
                return status;
            }
            std::string op_check;
            std::stringstream ss_check(current_cmd);
            ss_check >> op_check;
            std::transform(op_check.begin(), op_check.end(), op_check.begin(), ::toupper);
            if (op_check != "JE" && op_check != "JZ" && op_check != "JNE" && op_check != "JNZ" &&
                op_check != "JG" && op_check != "JL" && op_check != "JGE" && op_check != "JLE") {
                regs.set("EIP", regs.get("EIP") + 4);
            }
            usleep(1000000); // 1s delay
        }
        cpu.is_running = false;
        status = "RUN completed";
    } else {
        status = "RUN failed: No history";
    }

    if (!cpu.history.empty()) {
        cpu.history.push_back({cmd_addr, cmd});
    }
    return status;
}

std::string CommandHandler::cmdClear(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, mode;
    ss >> op >> mode;
    if (mode.empty()) mode = "ALL";
    std::transform(mode.begin(), mode.end(), mode.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (mode == "ALL" || mode == "REGS") {
        for (auto& reg : regs.getAll()) {
            if (reg.first == "ESP" && mode != "REGS") regs.set(reg.first, mem.STACK_TOP);
            else if (reg.first == "SP" && mode != "REGS") regs.set(reg.first, mem.STACK_TOP & 0xFFFF);
            else regs.set(reg.first, 0);
        }
        if (mode == "ALL") regs.set("EIP", CPU::PROGRAM_BASE);
    }
    if (mode == "ALL" || mode == "STACK") {
        mem.clear();
        regs.set("ESP", mem.STACK_TOP);
        regs.set("SP", mem.STACK_TOP & 0xFFFF);
    }
    if (mode == "ALL" || mode == "HISTORY") {
        cpu.clearHistory();
    }
    status = "CLEAR " + mode + " executed";

    if (!cpu.is_running && mode != "ALL") {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdMemset(const std::string& cmd, uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, addr_str;
    ss >> op >> addr_str;
    std::string addr_upper = addr_str;
    std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    try {
        uint32_t addr = std::stoul(addr_upper, nullptr, 16);
        if (memory_start_addr) *memory_start_addr = addr;
        auto mem_map = mem.getAllBytes();
        char debug_str[128];
        snprintf(debug_str, sizeof(debug_str), "MEMSET: Set to %08X: [%02x %02x %02x %02x %02x %02x]",
                 addr,
                 mem_map.count(addr) ? mem_map.at(addr) : 0,
                 mem_map.count(addr + 1) ? mem_map.at(addr + 1) : 0,
                 mem_map.count(addr + 2) ? mem_map.at(addr + 2) : 0,
                 mem_map.count(addr + 3) ? mem_map.at(addr + 3) : 0,
                 mem_map.count(addr + 4) ? mem_map.at(addr + 4) : 0,
                 mem_map.count(addr + 5) ? mem_map.at(addr + 5) : 0);
        status = debug_str;
    } catch (...) {
        status = "MEMSET failed: Invalid address";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdSettext(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, addr_str;
    ss >> op >> addr_str;

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (!addr_str.empty() && cmd.find('"') != std::string::npos) {
        try {
            std::string addr_upper = addr_str;
            std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
            uint32_t addr = std::stoul(addr_upper, nullptr, 16);
            size_t quote_start = cmd.find('"');
            size_t quote_end = cmd.find('"', quote_start + 1);
            if (quote_start != std::string::npos && quote_end != std::string::npos && quote_end > quote_start + 1) {
                std::string text = cmd.substr(quote_start + 1, quote_end - quote_start - 1);
                mem.writeText(addr, text);
                char debug_str[128];
                snprintf(debug_str, sizeof(debug_str),
                         "SETTEXT: Wrote \"%s\" at %s: [%02x %02x %02x %02x %02x %02x]",
                         text.c_str(), addr_str.c_str(),
                         mem.read(addr, true), mem.read(addr + 1, true), mem.read(addr + 2, true),
                         mem.read(addr + 3, true), mem.read(addr + 4, true), mem.read(addr + 5, true));
                status = debug_str;
            } else {
                status = "SETTEXT failed: Invalid text format";
            }
        } catch (...) {
            status = "SETTEXT failed: Invalid address";
        }
    } else {
        status = "SETTEXT failed: Missing address or text";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdMemview(const std::string& cmd, uint32_t* memory_start_addr) {
    std::stringstream ss(cmd);
    std::string op, addr_str;
    ss >> op >> addr_str;
    std::string addr_upper = addr_str;
    std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");
    std::string status;

    if (!addr_str.empty()) {
        try {
            uint32_t addr = std::stoul(addr_upper, nullptr, 16);
            char debug_str[128];
            snprintf(debug_str, sizeof(debug_str),
                     "MEMVIEW: View set to %08X: [%02x %02x %02x %02x %02x %02x]",
                     addr,
                     mem.read(addr, true), mem.read(addr + 1, true), mem.read(addr + 2, true),
                     mem.read(addr + 3, true), mem.read(addr + 4, true), mem.read(addr + 5, true));
            if (memory_start_addr) *memory_start_addr = addr;
            status = debug_str;
        } catch (...) {
            status = "MEMVIEW failed: Invalid address";
        }
    } else {
        status = "MEMVIEW failed: Missing address";
    }

    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return status;
}

std::string CommandHandler::cmdHelp(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    uint32_t cmd_addr = regs.get("EIP");
    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return "Commands: MOV Rn Rm/val/[Rm+off] or [Rm+off]/[addr] Rn/val, MOVB R8 [Rm+off]/[addr] or [Rm+off]/[addr] val, ADD/XOR/SUB/CMP Rn Rm/val or [Rm+off]/[addr] Rn, PUSH Rn, POP Rn, JE/JZ addr, JNE/JNZ addr, JG addr, JL addr, JGE addr, JLE addr, RUN, CLEAR [ALL/REGS/STACK/HISTORY], MEMSET addr, SETTEXT addr \"text\", MEMVIEW addr, QUIT";
}

std::string CommandHandler::cmdQuit(const std::string& cmd, [[maybe_unused]] uint32_t* memory_start_addr) {
    uint32_t cmd_addr = regs.get("EIP");
    if (!cpu.is_running) {
        cpu.history.push_back({cmd_addr, cmd});
        regs.set("EIP", cmd_addr + 4);
    }
    return "QUIT";
}

bool CommandHandler::parseMemoryAddress(const std::string& arg, std::string& reg_out, int32_t& offset_out) {
    if (arg.size() < 3 || arg[0] != '[' || arg.back() != ']') return false;
    std::string inner = arg.substr(1, arg.size() - 2);
    size_t plus_pos = inner.find('+');
    size_t minus_pos = inner.find('-');
    std::string reg_str, offset_str;

    if (plus_pos != std::string::npos) {
        reg_str = inner.substr(0, plus_pos);
        offset_str = inner.substr(plus_pos + 1);
        offset_out = std::stoi(offset_str, nullptr, 16);
    } else if (minus_pos != std::string::npos) {
        reg_str = inner.substr(0, minus_pos);
        offset_str = inner.substr(minus_pos + 1);
        offset_out = -std::stoi(offset_str, nullptr, 16);
    } else {
        reg_str = inner;
        offset_out = 0;
    }

    std::transform(reg_str.begin(), reg_str.end(), reg_str.begin(), ::toupper);
    if (regs.getAll().count(reg_str)) {
        reg_out = reg_str;
        return true;
    } else {
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
