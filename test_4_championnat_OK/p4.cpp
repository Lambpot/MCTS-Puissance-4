#include "p4.hpp"
#include <iostream>

P4::P4() : board(ROWS, std::vector<int>(COLS, 0)), lastMoveCol(-1), j1aletrait(true) {}

bool P4::isValidMove(int col) const {
    return col >= 0 && col < COLS && board[0][col] == 0;
}

void P4::makeMove(int col) {
    if (!isValidMove(col)) return;
    
    // 从底部开始找到第一个空位
    for (int row = ROWS - 1; row >= 0; row--) {
        if (board[row][col] == 0) {
            board[row][col] = j1aletrait ? 1 : 2;
            lastMoveCol = col;
            j1aletrait = !j1aletrait;
            break;
        }
    }
}

bool P4::checkWin() const {
    if (lastMoveCol == -1) return false;
    
    int lastMoveRow = -1;
    for (int row = 0; row < ROWS; row++) {
        if (board[row][lastMoveCol] != 0) {
            lastMoveRow = row;
            break;
        }
    }
    
    int player = board[lastMoveRow][lastMoveCol];
    
    // 检查水平方向
    int count = 0;
    for (int col = 0; col < COLS; col++) {
        if (board[lastMoveRow][col] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    // 检查垂直方向
    count = 0;
    for (int row = 0; row < ROWS; row++) {
        if (board[row][lastMoveCol] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    // 检查对角线方向
    // 主对角线
    count = 0;
    for (int i = -3; i <= 3; i++) {
        int row = lastMoveRow + i;
        int col = lastMoveCol + i;
        if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
            if (board[row][col] == player) {
                count++;
                if (count >= 4) return true;
            } else {
                count = 0;
            }
        }
    }
    
    // 副对角线
    count = 0;
    for (int i = -3; i <= 3; i++) {
        int row = lastMoveRow + i;
        int col = lastMoveCol - i;
        if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
            if (board[row][col] == player) {
                count++;
                if (count >= 4) return true;
            } else {
                count = 0;
            }
        }
    }
    
    return false;
}

bool P4::isDraw() const {
    for (int col = 0; col < COLS; col++) {
        if (board[0][col] == 0) return false;
    }
    return true;
}

unsigned P4::Nbcoups() {
    if (checkWin() || isDraw()) return 0;
    int count = 0;
    for (int col = 0; col < COLS; col++) {
        if (isValidMove(col)) count++;
    }
    return count;
}

Resultat P4::Eval() const {
    if (checkWin()) {
        return j1aletrait ? Resultat::j0gagne : Resultat::j1gagne;
    }
    if (isDraw()) {
        return Resultat::partieNulle;
    }
    return Resultat::partieNulle;  // 游戏未结束
}

void P4::EffectuerCoup(unsigned i) {
    int col = -1;
    int count = 0;
    for (int c = 0; c < COLS; c++) {
        if (isValidMove(c)) {
            if (count == i) {
                col = c;
                break;
            }
            count++;
        }
    }
    if (col != -1) {
        makeMove(col);
    }
}

std::string P4::ToString() const {
    std::string s = "\n";
    for (int row = 0; row < ROWS; row++) {
        s += "|";
        for (int col = 0; col < COLS; col++) {
            if (board[row][col] == 0) s += " ";
            else if (board[row][col] == 1) s += "X";
            else s += "O";
            s += "|";
        }
        s += "\n";
    }
    s += "+-+-+-+-+-+-+-+\n";
    s += " 0 1 2 3 4 5 6 \n";
    s += (j1aletrait ? "Player 1's turn" : "Player 2's turn");
    return s;
}

bool P4::operator==(const P4& other) const {
    return board == other.board && j1aletrait == other.j1aletrait;
} 