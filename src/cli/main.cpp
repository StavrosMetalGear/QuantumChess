#include <iostream>
#include <string>

#include "quantum_chess/Game.h"

int main() {
    quantum_chess::Game game;

    std::cout << "Quantum Chess\n";
    std::cout << "Enter moves like: e2 e4\n";
    std::cout << "Enter quit to exit.\n";

    while (true) {
        std::cout << game.board().toString();

        const bool whiteTurn =
            game.currentTurn() ==
            quantum_chess::PieceColor::White;

        if (game.isInCheck(game.currentTurn())) {
            std::cout << "\nCHECK!\n";
        }

        std::cout
            << "\n"
            << (whiteTurn ? "White" : "Black")
            << " move: ";

        std::string source;
        std::cin >> source;

        if (!std::cin ||
            source == "quit" ||
            source == "exit") {
            break;
        }

        std::string destination;
        std::cin >> destination;

        if (!std::cin) {
            break;
        }

        std::string errorMessage;

        if (!game.makeMove(
                source,
                destination,
                errorMessage)) {
            std::cout
                << "\nInvalid move: "
                << errorMessage
                << "\n";
        }
    }

    std::cout << "\nGame ended.\n";
    return 0;
}
