#include "Emulator.hpp"  // Include the Emulator class header

// Main function: Entry point of the CPU emulator program
int main() {
    Emulator emulator;  // Create an instance of the Emulator class
                        // This initializes the CPU, registers, memory, and screen components
    
    emulator.run();     // Start the emulator's main execution loop
                        // This handles user input, executes commands, and updates the UI until terminated
    
    return 0;           // Exit the program with a success status (0)
                        // Reached when the emulator loop exits (e.g., via QUIT command)
}
