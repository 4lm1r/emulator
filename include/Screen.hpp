#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <ncurses.h>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include "Memory.hpp"

class Screen {
public:
    Screen();
    ~Screen();
    void updateRegisters(const std::map<std::string, uint32_t>& regs, const std::string& changed_reg = "");
    void updateStack(const std::map<uint32_t, uint32_t>& mem, uint32_t esp); // Changed to take Memory&
    void updateMemoryAndHistory(const std::map<uint32_t, uint32_t>& mem, uint32_t start_addr,
                                const std::vector<std::pair<uint32_t, std::string>>& history);
    void updateStatus(const std::string& msg);
    std::string getInput();

private:
    WINDOW *reg_win, *stack_win, *input_win, *memory_win, *history_win, *status_rect_win;
    void initWindow(WINDOW*& win, int height, int width, int start_y, int start_x, int color_pair, const std::string& title);
};

#endif
