#include <iostream>
#include "quantum_chess/Game.h"

int main() {
    quantum_chess::Game game;

    std::cout << "Quantum Chess\n";
    std::cout << game.board().toString();

    return 0;
}
