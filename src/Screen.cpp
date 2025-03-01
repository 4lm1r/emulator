#include "Screen.hpp"
#include <sstream>    // For string stream formatting
#include <iomanip>    // For hex formatting (setw, setfill)
#include <map>        // For std::map (register and memory maps)
#include <cstring>    // For strlen and snprintf

// Constructor for Screen class
// Initializes the ncurses terminal interface and creates windows
Screen::Screen() {
    initscr();  // Initialize ncurses screen
    start_color();  // Enable color support
    // Define color pairs: foreground/background
    init_pair(1, COLOR_GREEN, COLOR_BLACK);   // Registers window
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // Highlights (e.g., changed register, ESP)
    init_pair(3, COLOR_CYAN, COLOR_BLACK);    // Other windows (input, memory, etc.)
    cbreak();   // Disable line buffering, process input immediately
    noecho();   // Do not echo typed characters to the screen
    keypad(stdscr, TRUE);  // Enable special keys (e.g., backspace)

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);  // Get terminal dimensions
    // Initialize windows with height, width, position, color, and title
    initWindow(reg_win, 14, max_x / 2, 0, 0, 1, "Registers");              // Left top: register display
    initWindow(stack_win, 14, max_x / 2, 0, max_x / 2, 1, "Stack (ESP)");  // Right top: stack display
    initWindow(input_win, 3, max_x, 14, 0, 2, "Input");                    // Middle: input area
    initWindow(memory_win, max_y - 22, max_x / 2, 17, 0, 3, "Memory");     // Left bottom: memory view
    initWindow(history_win, max_y - 22, max_x / 2, 17, max_x / 2, 3, "History");  // Right bottom: command history
    initWindow(status_rect_win, 5, max_x, max_y - 5, 0, 3, "Status");      // Bottom: status messages
}

// Destructor for Screen class
// Cleans up ncurses environment
Screen::~Screen() {
    endwin();  // Terminate ncurses and restore terminal
}

// Helper function to initialize a window
// Sets up a new ncurses window with specified properties
void Screen::initWindow(WINDOW*& win, int height, int width, int start_y, int start_x, int color_pair, const std::string& title) {
    win = newwin(height, width, start_y, start_x);  // Create new window
    wbkgd(win, COLOR_PAIR(color_pair));  // Set background color
    box(win, 0, 0);  // Draw a border around the window
    mvwprintw(win, 0, 1, "%s", title.c_str());  // Print title at top-left inside border
    wrefresh(win);  // Refresh to display changes
}

// Updates the register window with current register values
void Screen::updateRegisters(const std::map<std::string, uint32_t>& regs, const std::string& changed_reg) {
    wclear(reg_win);  // Clear the register window
    box(reg_win, 0, 0);  // Redraw border
    mvwprintw(reg_win, 0, 1, "Registers");  // Redraw title

    int x = 1, y = 1;  // Starting position inside window
    mvwprintw(reg_win, y++, x, "32-bit:");  // Section header for 32-bit registers
    x = 1;
    // Display 32-bit registers (EAX, EBX, ECX, EDX)
    for (const auto& reg : {"EAX", "EBX", "ECX", "EDX"}) {
        std::stringstream val_str;
        val_str << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << regs.at(reg);  // Format value as 8-digit hex
        if (reg == changed_reg) {  // Highlight if this register changed
            wattron(reg_win, A_BOLD | COLOR_PAIR(2));  // Bold yellow
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
            wattroff(reg_win, A_BOLD | COLOR_PAIR(2));
        } else {
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
        }
        x += 14;  // Move horizontally for next register
    }
    y++; x = 1;  // Next row
    // Display 32-bit registers (ESI, EDI, ESP, EBP)
    for (const auto& reg : {"ESI", "EDI", "ESP", "EBP"}) {
        std::stringstream val_str;
        val_str << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << regs.at(reg);
        if (reg == changed_reg) {
            wattron(reg_win, A_BOLD | COLOR_PAIR(2));
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
            wattroff(reg_win, A_BOLD | COLOR_PAIR(2));
        } else {
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
        }
        x += 14;
    }
    y++; x = 1;

    mvwprintw(reg_win, y++, x, "16-bit:");  // Section header for 16-bit registers
    // Display 16-bit registers (AX, BX, CX, DX, SI)
    for (const auto& reg : {"AX", "BX", "CX", "DX", "SI"}) {
        std::stringstream val_str;
        val_str << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << regs.at(reg);  // 4-digit hex
        if (reg == changed_reg) {
            wattron(reg_win, A_BOLD | COLOR_PAIR(2));
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
            wattroff(reg_win, A_BOLD | COLOR_PAIR(2));
        } else {
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
        }
        x += 10;
    }
    y++; x = 1;
    // Display 16-bit registers (DI, SP, BP)
    for (const auto& reg : {"DI", "SP", "BP"}) {
        std::stringstream val_str;
        val_str << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << regs.at(reg);
        if (reg == changed_reg) {
            wattron(reg_win, A_BOLD | COLOR_PAIR(2));
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
            wattroff(reg_win, A_BOLD | COLOR_PAIR(2));
        } else {
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
        }
        x += 10;
    }
    y++; x = 1;

    mvwprintw(reg_win, y++, x, "8-bit:");  // Section header for 8-bit registers
    // Display 8-bit registers (AH, AL, BH, BL, CH, CL, DH, DL)
    for (const auto& reg : {"AH", "AL", "BH", "BL", "CH", "CL", "DH", "DL"}) {
        std::stringstream val_str;
        val_str << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << regs.at(reg);  // 2-digit hex
        if (reg == changed_reg) {
            wattron(reg_win, A_BOLD | COLOR_PAIR(2));
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
            wattroff(reg_win, A_BOLD | COLOR_PAIR(2));
        } else {
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
        }
        x += 8;
    }
    y++; x = 1;

    mvwprintw(reg_win, y++, x, "Segment/Special:");  // Section header for segment and special registers
    // Display segment registers (CS, DS, SS, ES)
    for (const auto& reg : {"CS", "DS", "SS", "ES"}) {
        std::stringstream val_str;
        val_str << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << regs.at(reg);
        if (reg == changed_reg) {
            wattron(reg_win, A_BOLD | COLOR_PAIR(2));
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
            wattroff(reg_win, A_BOLD | COLOR_PAIR(2));
        } else {
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
        }
        x += 10;
    }
    y++; x = 1;
    // Display special registers (EIP, IP, FLAGS)
    for (const auto& reg : {"EIP", "IP", "FLAGS"}) {
        std::stringstream val_str;
        val_str << std::hex << std::uppercase << (reg[0] == 'E' ? std::setw(8) : std::setw(4)) << std::setfill('0') << regs.at(reg);
        if (reg == changed_reg) {
            wattron(reg_win, A_BOLD | COLOR_PAIR(2));
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
            wattroff(reg_win, A_BOLD | COLOR_PAIR(2));
        } else {
            mvwprintw(reg_win, y, x, "%s: %s", reg, val_str.str().c_str());
        }
        x += (reg[0] == 'E' ? 14 : 10);  // Wider spacing for 32-bit EIP
    }

    wrefresh(reg_win);  // Refresh to display updates
}

// Updates the stack window with values around the ESP address
void Screen::updateStack(const Memory& mem, uint32_t esp) {
    wclear(stack_win);  // Clear the stack window
    box(stack_win, 0, 0);  // Redraw border
    mvwprintw(stack_win, 0, 1, "Stack (ESP)");  // Redraw title

    int y = 1;
    mvwprintw(stack_win, y++, 1, "Top:");  // Label for stack top
    int max_y = getmaxy(stack_win) - 1;  // Prevent overflow
    // Display up to 10 32-bit values starting at ESP
    for (int i = 0; i < 10 && y < max_y; i++) {
        uint32_t addr = esp + (i * 4);  // Increment by 4 bytes (stack grows upward here for display)
        uint32_t val = mem.read(addr);  // Read 32-bit value from memory

        std::stringstream addr_str;
        addr_str << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << addr;
        std::stringstream val_str;
        val_str << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << val;
        if (addr == esp) {  // Highlight the current ESP position
            wattron(stack_win, A_BOLD | COLOR_PAIR(2));
            mvwprintw(stack_win, y++, 1, "%s: %s <- ESP", addr_str.str().c_str(), val_str.str().c_str());
            wattroff(stack_win, A_BOLD | COLOR_PAIR(2));
        } else {
            mvwprintw(stack_win, y++, 1, "%s: %s", addr_str.str().c_str(), val_str.str().c_str());
        }
    }
    wrefresh(stack_win);  // Refresh to display updates
}

// Updates the memory and history windows
void Screen::updateMemoryAndHistory(const std::map<uint32_t, uint8_t>& mem, uint32_t start_addr, const std::vector<std::pair<uint32_t, std::string>>& history) {
    // Memory section
    wclear(memory_win);  // Clear the memory window
    box(memory_win, 0, 0);  // Redraw border
    mvwprintw(memory_win, 0, 1, "Memory");  // Redraw title

    int y = 1;
    int max_y = getmaxy(memory_win) - 1;  // Prevent overflow
    int max_x = getmaxx(memory_win);  // Window width for truncation
    // Display memory in 16-byte rows starting at start_addr
    for (int i = 0; i < max_y - 1; i++) {
        uint32_t addr = start_addr + (i * 16);  // Each row increments by 16 bytes
        std::stringstream addr_str;
        addr_str << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << addr;
        std::stringstream hex_str;
        std::stringstream ascii_str;
        // Build hex and ASCII representation for 16 bytes
        for (int j = 0; j < 16; j++) {
            uint32_t byte_addr = addr + j;  // Address of each byte
            auto it = mem.find(byte_addr);
            uint8_t byte = (it != mem.end()) ? it->second : 0;  // Get byte or 0 if not present
            hex_str << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            if (j < 15) hex_str << " ";  // Space between bytes except last
            ascii_str << (byte >= 32 && byte <= 126 ? static_cast<char>(byte) : '.');  // Printable or dot
        }
        std::string line = addr_str.str() + ": " + hex_str.str() + "  " + ascii_str.str();
        if (line.length() > static_cast<size_t>(max_x - 2)) {  // Truncate if too long
            line = line.substr(0, max_x - 2);
        }
        mvwprintw(memory_win, y++, 1, "%s", line.c_str());
    }
    wrefresh(memory_win);  // Refresh to display updates

    // History section
    wclear(history_win);  // Clear the history window
    box(history_win, 0, 0);  // Redraw border
    mvwprintw(history_win, 0, 1, "History");  // Redraw title
    y = 1;
    // Display history in reverse order (most recent first)
    for (int i = history.size() - 1; i >= 0 && y < getmaxy(history_win) - 1; i--) {
        char line[256];
        snprintf(line, sizeof(line), "%08X: %s", history[i].first, history[i].second.c_str());  // Address: Command
        if (strlen(line) > static_cast<size_t>(getmaxx(history_win) - 2)) {  // Truncate if too long
            line[getmaxx(history_win) - 2] = '\0';
        }
        mvwprintw(history_win, y++, 1, "%s", line);
    }
    wrefresh(history_win);  // Refresh to display updates
}

// Updates the status window with a message
void Screen::updateStatus(const std::string& msg) {
    wclear(status_rect_win);  // Clear the status window
    box(status_rect_win, 0, 0);  // Redraw border
    mvwprintw(status_rect_win, 0, 1, "Status");  // Redraw title
    mvwprintw(status_rect_win, 1, 1, "%s", msg.c_str());  // Display status message
    wrefresh(status_rect_win);  // Refresh to display updates
}

// Retrieves user input from the input window
std::string Screen::getInput() {
    wclear(input_win);  // Clear the input window
    box(input_win, 0, 0);  // Redraw border
    mvwprintw(input_win, 0, 1, "Input");  // Redraw title
    mvwprintw(input_win, 1, 1, "> ");  // Prompt
    wmove(input_win, 1, 3);  // Move cursor after prompt
    wrefresh(input_win);  // Refresh to show prompt

    std::string input;  // Store user input
    int ch, pos = 3;  // Current character and cursor position
    while ((ch = wgetch(input_win)) != '\n') {  // Read characters until Enter
        if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
            if (!input.empty()) {
                input.pop_back();  // Remove last character
                pos--;
                wmove(input_win, 1, pos);
                waddch(input_win, ' ');  // Overwrite with space
                wmove(input_win, 1, pos);  // Move cursor back
            }
        } else if (ch >= 32 && ch <= 126) {  // Printable ASCII characters
            input += static_cast<char>(ch);  // Add to input string
            waddch(input_win, ch);  // Display character
            pos++;
        }
        wrefresh(input_win);  // Refresh after each change
    }
    return input;  // Return the completed input string
}
