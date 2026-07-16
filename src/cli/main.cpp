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

        const quantum_chess::PieceColor turn =
            game.currentTurn();

        const bool whiteTurn =
            turn == quantum_chess::PieceColor::White;

        if (game.isCheckmate(turn)) {
            std::cout << "\nCHECKMATE!\n";
            std::cout
                << (whiteTurn ? "Black" : "White")
                << " wins.\n";
            break;
        }

        if (game.isStalemate(turn)) {
            std::cout << "\nSTALEMATE!\n";
            std::cout << "The game is a draw.\n";
            break;
        }

        if (game.isInCheck(turn)) {
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
