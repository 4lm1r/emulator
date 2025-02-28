#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <map>
#include <string>
#include <cstdint> // Added for uint32_t

class Registers {
public:
    Registers();
    uint32_t get(const std::string& reg) const;
    void set(const std::string& reg, uint32_t val);
    const std::map<std::string, uint32_t>& getAll() const;

private:
    std::map<std::string, uint32_t> regs;
    void sync(const std::string& reg, uint32_t val);
};

#endif
