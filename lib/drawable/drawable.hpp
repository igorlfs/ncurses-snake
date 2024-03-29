#pragma once

#include <ncurses.h>
#include <utility>

namespace drawable {
class Drawable {
  public:
    /// Getters
    [[nodiscard]] chtype getChar() const { return this->character; }
    [[nodiscard]] std::pair<int, int> getHead() const { return this->head; }

    /// Setters
    void setChar(const char &cha, const int &n) {
        this->character = cha | A_BOLD | COLOR_PAIR(n);
    }
    void setHead(const std::pair<int, int> &location) { this->head = location; }

  private:
    std::pair<int, int> head;
    chtype character;
};
} // namespace drawable
