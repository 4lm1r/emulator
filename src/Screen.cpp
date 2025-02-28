#include "Screen.hpp"
#include <sstream>
#include <iomanip>
#include <map>      // Added for std::map
#include <cstring>  // Added for strlen

Screen::Screen() {
    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    initWindow(reg_win, 14, max_x / 2, 0, 0, 1, "Registers");
    initWindow(stack_win, 14, max_x / 2, 0, max_x / 2, 1, "Stack (ESP)");
    initWindow(input_win, 3, max_x, 14, 0, 2, "Input");
    initWindow(memory_win, max_y - 22, max_x / 2, 17, 0, 3, "Memory");
    initWindow(history_win, max_y - 22, max_x / 2, 17, max_x / 2, 3, "History");
    initWindow(status_rect_win, 5, max_x, max_y - 5, 0, 3, "Status");
}

Screen::~Screen() {
    endwin();
}

void Screen::initWindow(WINDOW*& win, int height, int width, int start_y, int start_x, int color_pair, const std::string& title) {
    win = newwin(height, width, start_y, start_x);
    wbkgd(win, COLOR_PAIR(color_pair));
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "%s", title.c_str());
    wrefresh(win);
}

void Screen::updateRegisters(const std::map<std::string, uint32_t>& regs, const std::string& changed_reg) {
    wclear(reg_win);
    box(reg_win, 0, 0);
    mvwprintw(reg_win, 0, 1, "Registers");

    int x = 1, y = 1;
    mvwprintw(reg_win, y++, x, "32-bit:");
    x = 1;
    for (const auto& reg : {"EAX", "EBX", "ECX", "EDX"}) {
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

    mvwprintw(reg_win, y++, x, "16-bit:");
    for (const auto& reg : {"AX", "BX", "CX", "DX", "SI"}) {
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

    mvwprintw(reg_win, y++, x, "8-bit:");
    for (const auto& reg : {"AH", "AL", "BH", "BL", "CH", "CL", "DH", "DL"}) {
        std::stringstream val_str;
        val_str << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << regs.at(reg);
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

    mvwprintw(reg_win, y++, x, "Segment/Special:");
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
        x += (reg[0] == 'E' ? 14 : 10);
    }

    wrefresh(reg_win);
}

void Screen::updateStack(const std::map<uint32_t, uint32_t>& mem, uint32_t esp) {
    wclear(stack_win);
    box(stack_win, 0, 0);
    mvwprintw(stack_win, 0, 1, "Stack (ESP)");

    int y = 1;
    mvwprintw(stack_win, y++, 1, "Top:");
    for (int i = 0; i < 10 && y < 13; i++) {
        uint32_t addr = esp + (i * 4);
        auto it = mem.find(addr);
        uint32_t val = 0; //Default value if not found.
        if(it != mem.end()){
            val = it->second;
        }

        std::stringstream addr_str;
        addr_str << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << addr;
        std::stringstream val_str;
        val_str << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << val;
        if (addr == esp) {
            wattron(stack_win, A_BOLD | COLOR_PAIR(2));
            mvwprintw(stack_win, y++, 1, "%s: %s <- ESP", addr_str.str().c_str(), val_str.str().c_str());
            wattroff(stack_win, A_BOLD | COLOR_PAIR(2));
        } else {
            mvwprintw(stack_win, y++, 1, "%s: %s", addr_str.str().c_str(), val_str.str().c_str());
        }
    }
    wrefresh(stack_win);
}

void Screen::updateMemoryAndHistory(const std::map<uint32_t, uint32_t>& mem, uint32_t start_addr,
                                     const std::vector<std::pair<uint32_t, std::string>>& history) {
    wclear(memory_win);
    box(memory_win, 0, 0);
    mvwprintw(memory_win, 0, 1, "Memory");

    int y = 1;
    int max_y = getmaxy(memory_win) - 1;
    int max_x = getmaxx(memory_win);
    for (int i = 0; i < max_y - 1; i++) {
        uint32_t addr = start_addr + (i * 16);
        std::stringstream addr_str;
        addr_str << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << addr;
        std::stringstream hex_str;
        std::stringstream ascii_str;
        for (int j = 0; j < 16; j++) {
            uint32_t byte_addr = addr; //The base address for the 16 byte block.
            auto it = mem.find(byte_addr);
            uint8_t byte = 0;

            if (it != mem.end()) {
                uint32_t value = it->second;
                byte = (value >> (j * 8)) & 0xFF; //Extract the jth byte.
            }

            hex_str << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            if (j < 15) hex_str << " ";
            ascii_str << (byte >= 32 && byte <= 126 ? static_cast<char>(byte) : '.');
        }
        std::string line = addr_str.str() + ": " + hex_str.str() + "  " + ascii_str.str();
        if (line.length() > static_cast<size_t>(max_x - 2)) {
            line = line.substr(0, max_x - 2);
        }
        mvwprintw(memory_win, y++, 1, "%s", line.c_str());
    }
    wrefresh(memory_win);


    wclear(history_win);
    box(history_win, 0, 0);
    mvwprintw(history_win, 0, 1, "History");
    y = 1;
    for (int i = history.size() - 1; i >= 0 && y < getmaxy(history_win) - 1; i--) {
        char line[256];
        snprintf(line, sizeof(line), "%08X: %s", history[i].first, history[i].second.c_str());
        if (strlen(line) > static_cast<size_t>(getmaxx(history_win) - 2)) {
            line[getmaxx(history_win) - 2] = '\0';
        }
        mvwprintw(history_win, y++, 1, "%s", line);
    }
    wrefresh(history_win);
}

void Screen::updateStatus(const std::string& msg) {
    wclear(status_rect_win);
    box(status_rect_win, 0, 0);
    mvwprintw(status_rect_win, 0, 1, "Status");
    mvwprintw(status_rect_win, 1, 1, "%s", msg.c_str());
    wrefresh(status_rect_win);
}

std::string Screen::getInput() {
    wclear(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 0, 1, "Input");
    mvwprintw(input_win, 1, 1, "> ");
    wmove(input_win, 1, 3);
    wrefresh(input_win);

    std::string input;
    int ch, pos = 3;
    while ((ch = wgetch(input_win)) != '\n') {
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (!input.empty()) {
                input.pop_back();
                pos--;
                wmove(input_win, 1, pos);
                waddch(input_win, ' ');
                wmove(input_win, 1, pos);
            }
        } else if (ch >= 32 && ch <= 126) {
            input += static_cast<char>(ch);
            waddch(input_win, ch);
            pos++;
        }
        wrefresh(input_win);
    }
    return input;
}
