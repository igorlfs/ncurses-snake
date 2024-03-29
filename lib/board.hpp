#pragma once

#include <ncurses.h>
#include <utility>

class Board {
  public:
    /// Constructors
    Board(WINDOW *win);

    /// Getters
    [[nodiscard]] int getX() const { return this->xMax - 1; }
    [[nodiscard]] int getY() const { return this->yMax - 1; }
    [[nodiscard]] int getSize() const {
        return (this->xMax - 1) * (this->yMax - 1);
    }
    WINDOW *getWin() { return this->gameWindow; }

    /// Lookup
    [[nodiscard]] bool isOutOfBounds(const std::pair<int, int> &location) const;

    /// Operations
    void print(const std::pair<int, int> &location, const chtype &cha) const;

  private:
    WINDOW *gameWindow;
    int yMax, xMax;
};
