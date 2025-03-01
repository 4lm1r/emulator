#include "Memory.hpp"

// Constructor for Memory class
// Initializes memory with a default value (likely unused in this context due to map-based implementation)
Memory::Memory(uint32_t value) : value_(value) {}

// Getter for the stored value_ member
// Returns the internal value (appears unused based on the code provided)
uint32_t Memory::getValue() const {
   return value_;  // Returns the stored 32-bit value
}

// Setter for the stored value_ member
// Updates the internal value (appears unused based on the code provided)
void Memory::setValue(uint32_t value) {
   value_ = value;  // Sets the internal 32-bit value
}

// Writes a value to memory at the specified address
// Supports both byte (8-bit) and word (32-bit) writes
void Memory::write(uint32_t addr, uint32_t val, bool is_byte) {
    if (is_byte) {  // Byte write mode
        mem[addr] = val & 0xFF;  // Store only the least significant byte (8 bits)
    } else {  // Word write mode (32-bit)
        mem[addr] = val & 0xFF;              // Store byte 0 (LSB)
        mem[addr + 1] = (val >> 8) & 0xFF;  // Store byte 1
        mem[addr + 2] = (val >> 16) & 0xFF; // Store byte 2
        mem[addr + 3] = (val >> 24) & 0xFF; // Store byte 3 (MSB)
    }
}

// Reads a value from memory at the specified address
// Supports both byte (8-bit) and word (32-bit) reads
uint32_t Memory::read(uint32_t addr, bool is_byte) const {
    if (is_byte) {  // Byte read mode
        // Return the byte at addr if it exists, otherwise return 0
        return mem.count(addr) ? mem.at(addr) & 0xFF : 0;
    }
    uint32_t val = 0;  // Word read mode (32-bit)
    // Reconstruct 32-bit value from 4 consecutive bytes, defaulting to 0 if byte not present
    val |= (mem.count(addr) ? mem.at(addr) : 0);                // Byte 0 (LSB)
    val |= (mem.count(addr + 1) ? mem.at(addr + 1) : 0) << 8;  // Byte 1
    val |= (mem.count(addr + 2) ? mem.at(addr + 2) : 0) << 16; // Byte 2
    val |= (mem.count(addr + 3) ? mem.at(addr + 3) : 0) << 24; // Byte 3 (MSB)
    return val;  // Return the reconstructed 32-bit value
}

// Erases a specific memory address
void Memory::erase(uint32_t addr) {
    mem.erase(addr);  // Remove the byte at the specified address from the map
}

// Clears all memory contents
void Memory::clear() {
    mem.clear();  // Remove all entries from the memory map
}

// Returns a map of all memory bytes
// Converts internal memory representation to a byte-wise map
std::map<uint32_t, uint8_t> Memory::getAllBytes() const {
    std::map<uint32_t, uint8_t> byteMap;  // Resulting byte map
    for (const auto& pair : mem) {  // Iterate over internal memory map
        byteMap[pair.first] = static_cast<uint8_t>(pair.second & 0xFF);  // Store each byte
    }
    return byteMap;  // Return the byte-wise memory map
}

// Returns a map of all memory contents as 32-bit values
// Note: This function seems inconsistent with the byte-based mem map; possibly outdated or unused
std::map<uint32_t, uint32_t> Memory::getAll() const {
    std::map<uint32_t, uint32_t> memoryMap;  // Resulting 32-bit value map
    for (const auto& pair : memory_) {  // Iterate over memory_ (likely a typo/mismatch with mem)
        memoryMap[pair.first] = pair.second.getValue();  // Store 32-bit values
    }
    return memoryMap;  // Return the 32-bit memory map
}

// Writes a string to memory as consecutive bytes
void Memory::writeText(uint32_t addr, const std::string& text) {
    for (size_t i = 0; i < text.length(); i++) {
        mem[addr + i] = static_cast<uint8_t>(text[i]);  // Write each character as a byte
    }
    mem[addr + text.length()] = 0;  // Append null terminator byte
}
