#include "jhp4.hpp"
#include <iostream>

JHP4::JHP4() {
    this->nom = "Human Player";
}

unsigned JHP4::Jouer(P4 p) {
    int col;
    do {
        std::cout << "Enter column (0-6): ";
        std::cin >> col;
    } while (col < 0 || col > 6 || !p.isValidMove(col));
    return col;
} 