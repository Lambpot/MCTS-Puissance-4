#pragma once

#include <vector>
#include <string>
#include "projet.hpp"

class P4 {
private:
    static const int ROWS = 6;
    static const int COLS = 7;
    std::vector<std::vector<int>> board;  // 0: empty, 1: player 1, 2: player 2
    int lastMoveCol;  // 记录最后一手棋的列号

public:
    bool j1aletrait;  // true if player 1's turn

    P4();
    bool isValidMove(int col) const;
    void makeMove(int col);
    bool checkWin() const;
    bool isDraw() const;
    unsigned Nbcoups();
    Resultat Eval() const;
    void EffectuerCoup(unsigned i);
    std::string ToString() const;
    bool operator==(const P4& other) const;
}; 