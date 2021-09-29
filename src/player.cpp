#include "player.hpp"
#include <cstdlib>
#include <curses.h>
#include <map>
#include <random>
#include <vector>

// https://stackoverflow.com/a/13445752
namespace std {
int randomNumber(const int &floor, const int &ceiling) {
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> dist(floor, ceiling);
    return dist(rng);
}
} // namespace std

using namespace Cobra;

cobra::cobra(WINDOW *win, const int &c) : currentWin(win), character(c) {
    getmaxyx(this->currentWin, this->yMax, this->xMax);
    keypad(this->currentWin, true);
    this->spawnCobra();
    this->spawnFood();
}
void cobra::spawnCobra() {
    clear();
    for (int i = 0; i < (this->xMax - 2) / 4 + 1; ++i) {
        this->locations.push_front({1, i + (this->xMax - 2) / 4});
        mvwaddch(this->currentWin, 1, i + (this->xMax - 2) / 4,
                 this->character);
    }
}
void cobra::updateDirection(const int &input) {
    std::map<int, directions> invalidMoves = {{KEY_UP, VERTICAL},
                                              {KEY_DOWN, VERTICAL},
                                              {KEY_LEFT, HORIZONTAL},
                                              {KEY_RIGHT, HORIZONTAL}};
    for (auto i = invalidMoves.begin(); i != invalidMoves.end(); ++i) {
        if (input == i->first && i->second != this->direction) {
            this->input = input;
            this->direction = i->second;
            return;
        }
    }
}
void cobra::move() {
    pair<int, int> newHead = moveHelper();
    if (newHead != this->food) { // "Eat" by not removing tail
        mvwaddch(this->currentWin, this->locations.back().first,
                 this->locations.back().second, BLANK);
        this->locations.pop_back();
    } // You need to remove the tail before checking if the head is inside
    if (isInsideCobra(newHead)) gameOver("YOU LOST!");
    this->locations.push_front(newHead);
    const int size = (this->yMax - 2) * (this->xMax - 2);
    if (newHead == this->food && this->locations.size() != size)
        this->spawnFood();
    if (this->locations.size() == size) this->gameOver("YOU WON!");
    mvwaddch(this->currentWin, this->locations.front().first,
             this->locations.front().second, this->character);
}
std::pair<int, int> cobra::moveHelper() const {
    pair<int, int> head = this->locations.front();
    switch (this->input) {
        case KEY_UP: head = {head.first - 1, head.second}; break;
        case KEY_DOWN: head = {head.first + 1, head.second}; break;
        case KEY_LEFT: head = {head.first, head.second - 1}; break;
        case KEY_RIGHT: head = {head.first, head.second + 1}; break;
    }
    if (boundaryCheck(head.first, head.second)) gameOver("YOU LOST!");
    return head;
}
void cobra::spawnFood() {
    std::vector<pair<int, int>> positions;
    for (int i = 1; i <= this->yMax - 2; ++i)
        for (int j = 1; j <= this->xMax - 2; ++j) {
            pair<int, int> pair = {i, j};
            if (!isInsideCobra(pair)) positions.push_back(pair);
        }
    int index = std::randomNumber(0, positions.size() - 1);
    this->food = positions[index];
    mvwaddch(this->currentWin, this->food.first, this->food.second,
             this->foodChar);
}
bool cobra::isInsideCobra(const pair<int, int> &cell) const {
    for (auto i : this->locations)
        if (i == cell) return true;
    return false;
}
bool cobra::boundaryCheck(const int &m, const int &n) const {
    return (m <= 0 || m >= this->yMax - 1 || n <= 0 || n >= this->xMax - 1);
}
void cobra::gameOver(const std::string &message) const {
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    int x = message.size() + 2; // Message + Borders
    WINDOW *gameOver = newwin(3, x, (yMax / 2) - 2, (xMax - x) / 2);
    box(gameOver, 0, 0);
    refresh();
    mvwprintw(gameOver, 1, 1, message.c_str());
    wgetch(gameOver);
    delwin(gameOver);
    delwin(this->currentWin);
    endwin();
    std::exit(1);
}
