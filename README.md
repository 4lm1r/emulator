# CPU Emulator

This project is a **CPU emulator** written in C++ that simulates a simple processor with a terminal-based interface powered by the `ncurses` library. It emulates CPU operations, memory management, and register handling, providing a lightweight and interactive way to explore low-level computing concepts.

## Features
- Emulates a CPU with basic instruction processing.
- Terminal-based user interface using `ncurses` for real-time interaction.
- Modular design with separate components for CPU, memory, registers, and screen handling.
- Written in C++17 for modern performance and portability.

## Project Structure
The project is organized into multiple source files under the `src/` directory, with headers in `include/`:

- **`src/main.cpp`**: The entry point of the program. It initializes and loads the core classes to start the emulator.
- **`src/CPU.cpp`**: Manages CPU processes, including instruction execution and state handling.
- **`src/Screen.cpp`**: Handles the terminal interface and rendering using the `ncurses` library.
- **`src/Memory.cpp`**: Manages memory access, allocation, and related operations.
- **`src/Registers.cpp`**: Controls register management and operations.
- **`src/Emulator.cpp`**: Orchestrates the entire emulation process, tying together the CPU, memory, registers, and screen components.
- **`include/`**: Contains header files for the classes (e.g., `CPU.hpp`, `Screen.hpp`, etc.).

## Prerequisites
To build and run this project, you’ll need:
- A C++ compiler supporting C++17 (e.g., `g++` 7.0 or later).
- The `ncurses` library installed on your system.
  - On Ubuntu/Debian: `sudo apt-get install libncurses5-dev libncursesw5-dev`
  - On Fedora: `sudo dnf install ncurses-devel`
  - On macOS: `brew install ncurses` (with Homebrew)
- **CMake** (version 3.10 or higher) for building the project.
  - Install via: `sudo apt-get install cmake` (Ubuntu/Debian), `sudo dnf install cmake` (Fedora), or `brew install cmake` (macOS).

## Building the Project
This project uses **CMake** to manage the build process. The build is configured with C++17, debug flags (`-g`), and warnings enabled (`-Wall -Wextra`). Follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/4lm1r/emulator.git
   cd emulator
   
2. Create a build directory and navigate into it:
   bash
   mkdir build
   cd build
   
3. Run CMake to configure the project:
   bash
   cmake ..
      
4. Build the project:
   bash
   make
   
5. Run the emulator:
   bash
   ./emulator

# Usage

  Once launched, the emulator provides a terminal-based interface via ncurses. Use the keyboard to
  interact with the emulator (specific controls will depend on the implemented instruction set and UI
  design—update this section as needed).

# Contributing

  Contributions are welcome! Feel free to:
          Open issues for bugs or feature requests.
          Submit pull requests with improvements or fixes.

# License

  This project is licensed under the GPL-2.0 License.
