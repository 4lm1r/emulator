#include "CPU.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <set>
#include <unistd.h>
#include <cstring> // For snprintf

CPU::CPU(Registers& r, Memory& m) : regs(r), mem(m), is_running(false) {
    regs.set("EIP", PROGRAM_BASE);
}

std::string CPU::execute(const std::string& cmd, uint32_t* memory_start_addr) {
    std::string status;
    std::string op, reg1, reg2;
    std::stringstream ss(cmd);
    ss >> op >> reg1;

    std::string op_upper = op;
    std::string reg1_upper = reg1;
    std::transform(op_upper.begin(), op_upper.end(), op_upper.begin(), ::toupper);
    std::transform(reg1_upper.begin(), reg1_upper.end(), reg1_upper.begin(), ::toupper);

    uint32_t cmd_addr = regs.get("EIP");

    if (op_upper == "MOV") {
        ss >> reg2;
        if (reg1[0] == '[') {
            std::string mem_reg;
            int32_t offset;
            if (parseMemoryAddress(reg1, mem_reg, offset)) {
                std::string reg2_upper = reg2;
                std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
                uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                uint32_t val;
                if (regs.getAll().count(reg2_upper)) {
                    val = regs.get(reg2_upper);
                    mem.write(addr, val);
                    char debug_str[64];
                    snprintf(debug_str, sizeof(debug_str), "MOV [%08X] <- %08X", addr, val);
                    status = debug_str;
                    if (!is_running) {
                        history.push_back({cmd_addr, cmd});
                        regs.set("EIP", cmd_addr + 4);
                    }
                } else {
                    try {
                        val = std::stoul(reg2_upper, nullptr, 16);
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
        } else if (regs.getAll().count(reg1_upper)) {
            std::string reg2_upper = reg2;
            std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
            if (regs.getAll().count(reg2_upper)) {
                uint32_t val = regs.get(reg2_upper);
                regs.set(reg1_upper, val);
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "MOV %s <- %08X", reg1_upper.c_str(), val);
                status = debug_str;
                if (!is_running) {
                    history.push_back({cmd_addr, cmd});
                    regs.set("EIP", cmd_addr + 4);
                }
            } else if (reg2[0] == '[') {
                std::string mem_reg;
                int32_t offset;
                if (parseMemoryAddress(reg2, mem_reg, offset)) {
                    uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                    uint32_t mem_val = mem.read(addr);
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
            } else if (!reg2.empty()) {
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
    } else if (op_upper == "MOVB") {
        ss >> reg2;
        if (reg1[0] == '[') {
            std::string mem_reg;
            int32_t offset;
            if (parseMemoryAddress(reg1, mem_reg, offset)) {
                std::string reg2_upper = reg2;
                std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
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
        } else if (regs.getAll().count(reg1_upper)) {
            std::set<std::string> byte_registers = {"AH", "AL", "BH", "BL", "CH", "CL", "DH", "DL"};
            if (byte_registers.count(reg1_upper) == 0) {
                status = "MOVB failed: Not a byte register";
            } else {
                std::string reg2_upper = reg2;
                std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
                if (reg2[0] == '[') {
                    std::string mem_reg;
                    int32_t offset;
                    if (parseMemoryAddress(reg2, mem_reg, offset)) {
                        uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                        uint32_t mem_val = mem.read(addr, true);
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
    } else if (op_upper == "ADD" || op_upper == "XOR" || op_upper == "SUB" || op_upper == "CMP") {
        ss >> reg2;
        if (reg1[0] == '[') {
            std::string mem_reg;
            int32_t offset;
            if (parseMemoryAddress(reg1, mem_reg, offset)) {
                std::string reg2_upper = reg2;
                std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
                if (!regs.getAll().count(reg2_upper)) {
                    status = op_upper + " failed: Invalid register";
                } else {
                    uint32_t addr = mem_reg.empty() ? offset : (regs.get(mem_reg) + offset);
                    uint32_t val1 = mem.read(addr);
                    uint32_t val2 = regs.get(reg2_upper);
                    uint32_t result_val;
                    uint32_t flags = 0;

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
                        mem.write(addr, result_val);
                        char debug_str[64];
                        snprintf(debug_str, sizeof(debug_str), "ADD [%08X]: %08X + %08X = %08X", 
                                 addr, val1, val2, result_val);
                        status = debug_str;
                    } else if (op_upper == "XOR") {
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
                        mem.write(addr, result_val);
                        char debug_str[64];
                        snprintf(debug_str, sizeof(debug_str), "SUB [%08X]: %08X - %08X = %08X", 
                                 addr, val1, val2, result_val);
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
                        char debug_str[64];
                        snprintf(debug_str, sizeof(debug_str), "CMP [%08X] - %08X: ZF=%d SF=%d OF=%d", 
                                 addr, val2, (flags & ZF) != 0, (flags & SF) != 0, (flags & OF) != 0);
                        status = debug_str;
                    }
                    if (!is_running) {
                        history.push_back({cmd_addr, cmd});
                        regs.set("EIP", cmd_addr + 4);
                    }
                }
            } else {
                status = op_upper + " failed: Invalid memory address";
            }
        } else if (regs.getAll().count(reg1_upper)) {
            uint32_t val1 = regs.get(reg1_upper);
            uint32_t val2;
            uint32_t result_val;
            uint32_t flags = 0;

            std::string reg2_upper = reg2;
            std::transform(reg2_upper.begin(), reg2_upper.end(), reg2_upper.begin(), ::toupper);
            if (regs.getAll().count(reg2_upper)) {
                val2 = regs.get(reg2_upper);
            } else {
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
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "CMP %s - %08X: ZF=%d SF=%d OF=%d", 
                         reg1_upper.c_str(), val2, (flags & ZF) != 0, (flags & SF) != 0, (flags & OF) != 0);
                status = debug_str;
            }
            if (!is_running) {
                history.push_back({cmd_addr, cmd});
                regs.set("EIP", cmd_addr + 4);
            }
        } else {
            status = op_upper + " failed: Invalid register";
        }
    } else if (op_upper == "PUSH") {
        if (regs.getAll().count(reg1_upper)) {
            uint32_t val = regs.get(reg1_upper);
            uint32_t esp = regs.get("ESP");
            char debug_str[128];
            snprintf(debug_str, sizeof(debug_str), "PUSH %s: ESP=%08X val=%08X STACK_BASE=%08X", 
                     reg1_upper.c_str(), esp, val, mem.STACK_BASE);
            status = debug_str;

            if (esp > mem.STACK_BASE) {
                esp -= 4;
                mem.write(esp, val);
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
    } else if (op_upper == "POP") {
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
    } else if (op_upper == "JE" || op_upper == "JZ") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                if (regs.get("FLAGS") & ZF) {
                    regs.set("EIP", target_addr);
                    status = "JE/JZ jumped to " + reg1;
                } else {
                    status = "JE/JZ no jump";
                    if (!is_running) regs.set("EIP", cmd_addr + 4);
                }
                if (!is_running) history.push_back({cmd_addr, cmd});
            } catch (...) {
                status = "JE/JZ failed: Invalid address";
            }
        } else {
            status = "JE/JZ failed: Missing address";
        }
    } else if (op_upper == "JNE" || op_upper == "JNZ") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                if (!(regs.get("FLAGS") & ZF)) {
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
    } else if (op_upper == "JG") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                bool zf = regs.get("FLAGS") & ZF;
                bool sf = regs.get("FLAGS") & SF;
                bool of = regs.get("FLAGS") & OF;
                if (!zf && (sf == of)) {
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
    } else if (op_upper == "JL") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                bool sf = regs.get("FLAGS") & SF;
                bool of = regs.get("FLAGS") & OF;
                if (sf != of) {
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
    } else if (op_upper == "JGE") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                bool sf = regs.get("FLAGS") & SF;
                bool of = regs.get("FLAGS") & OF;
                if (sf == of) {
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
    } else if (op_upper == "JLE") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t target_addr = std::stoul(addr_upper, nullptr, 16);
                bool zf = regs.get("FLAGS") & ZF;
                bool sf = regs.get("FLAGS") & SF;
                bool of = regs.get("FLAGS") & OF;
                if (zf || (sf != of)) {
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
    } else if (op_upper == "RUN") {
        if (!history.empty()) {
            is_running = true;
            regs.set("EIP", PROGRAM_BASE);
            uint32_t program_end = PROGRAM_BASE + (history.size() * 4);
            while (regs.get("EIP") >= PROGRAM_BASE && regs.get("EIP") < program_end) {
                size_t index = (regs.get("EIP") - PROGRAM_BASE) / 4;
                if (index >= history.size()) break;
                std::string current_cmd = history[index].second;
                status = execute(current_cmd, memory_start_addr); // Recursive call
                if (status == "QUIT") {
                    is_running = false;
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
                usleep(1000000); // 1s delay for visibility
            }
            is_running = false;
            status = "RUN completed";
            if (!history.empty()) {
                history.push_back({cmd_addr, cmd}); // Record RUN command
            }
        } else {
            status = "RUN failed: No history";
        }
    } else if (op_upper == "CLEAR") {
        std::string mode = reg1;
        if (mode.empty()) mode = "ALL";
        std::transform(mode.begin(), mode.end(), mode.begin(), ::toupper);
        if (mode == "ALL" || mode == "REGS") {
            for (auto& reg : regs.getAll()) {
                if (reg.first == "ESP" && mode != "REGS") regs.set(reg.first, mem.STACK_TOP);
                else if (reg.first == "SP" && mode != "REGS") regs.set(reg.first, mem.STACK_TOP & 0xFFFF);
                else regs.set(reg.first, 0);
            }
            if (mode == "ALL") regs.set("EIP", 0); // Reset EIP to 0 for ALL
        }
        if (mode == "ALL" || mode == "STACK") {
            mem.clear();
            regs.set("ESP", mem.STACK_TOP);
            regs.set("SP", mem.STACK_TOP & 0xFFFF);
        }
        if (mode == "ALL" || mode == "HISTORY") {
            clearHistory();
        }
        status = "CLEAR " + mode + " executed";
        if (!is_running && mode != "ALL") { // Don’t add CLEAR ALL to history
            history.push_back({cmd_addr, cmd});
            regs.set("EIP", cmd_addr + 4);
        }
    } else if (op_upper == "MEMSET") {
        if (!reg1.empty()) {
            try {
                std::string addr_upper = reg1;
                std::transform(addr_upper.begin(), addr_upper.end(), addr_upper.begin(), ::toupper);
                uint32_t addr = std::stoul(addr_upper, nullptr, 16);
                if (memory_start_addr) *memory_start_addr = addr;
                char debug_str[64];
                snprintf(debug_str, sizeof(debug_str), "MEMSET: Memory view set to %08X", addr);
                status = debug_str;
                if (!is_running) {
                    history.push_back({cmd_addr, cmd});
                    regs.set("EIP", cmd_addr + 4);
                }
            } catch (...) {
                status = "MEMSET failed: Invalid address";
            }
        } else {
            status = "MEMSET failed: Missing address";
        }
    } else if (op_upper == "SETTEXT") {
        if (!reg1.empty() && cmd.find('"') != std::string::npos) {
            try {
                std::string addr_str = reg1;
                std::transform(addr_str.begin(), addr_str.end(), addr_str.begin(), ::toupper);
                uint32_t addr = std::stoul(addr_str, nullptr, 16);
                size_t quote_start = cmd.find('"');
                size_t quote_end = cmd.find('"', quote_start + 1);
                if (quote_start != std::string::npos && quote_end != std::string::npos && quote_end > quote_start + 1) {
                    std::string text = cmd.substr(quote_start + 1, quote_end - quote_start - 1);
                    mem.writeText(addr, text);
                    status = "SETTEXT: Wrote \"" + text + "\" at " + reg1;
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
    } else if (op_upper == "HELP") {
        status = "HELP: See emulator for full command list";
        if (!is_running) {
            history.push_back({cmd_addr, cmd});
            regs.set("EIP", cmd_addr + 4);
        }
    } else if (op_upper == "QUIT") {
        if (!is_running) {
            history.push_back({cmd_addr, cmd});
            regs.set("EIP", cmd_addr + 4);
        }
        status = "QUIT";
    } else {
        status = "Unknown command: " + cmd;
    }

    return status;
}

std::vector<std::pair<uint32_t, std::string>>& CPU::getHistory() {
    return history;
}

void CPU::clearHistory() {
    history.clear();
}

bool CPU::parseMemoryAddress(const std::string& arg, std::string& reg_out, int32_t& offset_out) {
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

void CPU::runHistory() {
    // Implemented within RUN command
}
