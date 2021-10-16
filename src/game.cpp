#include "game.hpp"

#include <map>
#include <random>

// https://stackoverflow.com/a/13445752
int randomNumber(const int &f, const int &c) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(f, c);
    return dist(rng);
}

using namespace game;
Game::Game(WINDOW *win) : board(win), snake(), food() {
    this->spawnSnake();
    this->spawnFood();
    this->board.print(this->snake.getHead(), this->snake.getChar());
    for (std::pair i : this->snake.getBody())
        board.print(i, this->snake.getChar());
    this->board.print(this->food.getHead(), this->food.getChar());
}
void Game::processInput() {
    int input = wgetch(this->board.getWin());
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
void Game::update() {
    std::pair<int, int> newHead = this->snake.changeHead(this->input);
    if (this->board.isOutOfBounds(newHead)) this->gameOver("DEFEAT!");
    // "Eat" by not removing tail
    if (newHead != this->food.getHead()) this->snake.removeTail();
    // You need to remove the tail before checking if the head is inside
    if (this->snake.isInBody(newHead)) this->gameOver("DEFEAT!");
    this->snake.insertHead(newHead);
    int snakeSize = this->snake.getBody().size() + 1; // size body + head
    if (newHead == this->food.getHead() && snakeSize != this->board.getSize())
        this->spawnFood();
    if (snakeSize == this->board.getSize()) this->gameOver("VICTORY!");
}
void Game::print() {
    this->board.print(this->snake.getOldTail(), BLANK);
    this->board.print(this->food.getHead(), this->food.getChar());
    this->board.print(this->snake.getHead(), this->snake.getChar());
    wrefresh(this->board.getWin());
}
void Game::spawnSnake() {
    constexpr int y = 1;
    const int x = this->board.getX() / 2;
    this->snake.setHead({y, x});
    std::list<std::pair<int, int>> body;
    for (int i = 1; i < x; ++i) body.push_front({y, i});
    this->snake.setBody(body);
}
void Game::spawnFood() {
    std::vector<std::pair<int, int>> validPositions;
    for (int i = 1; i <= this->board.getY(); ++i)
        for (int j = 1; j <= this->board.getX(); ++j) {
            std::pair<int, int> candidate = {i, j};
            if (!snake.isInBody(candidate) && candidate != snake.getHead())
                validPositions.push_back(candidate);
        }
    int index = randomNumber(0, validPositions.size() - 1);
    this->food.setHead(validPositions[index]);
}
void Game::gameOver(const std::string &message) {
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    int x = message.size() + 2; // Message + Borders
    WINDOW *gameOver = newwin(3, x, (yMax - 3) / 2, (xMax - x) / 2);
    nocbreak(); // Disables half-delay mode
    box(gameOver, 0, 0);
    mvwprintw(gameOver, 1, 1, message.c_str());
    wgetch(gameOver);
    delwin(gameOver);
    this->isGameOver = true;
}
