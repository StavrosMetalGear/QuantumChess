#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "quantum_chess/Game.h"

int main() {
    quantum_chess::Game game;

    std::cout << "Quantum Chess\n";
    std::cout << "Normal move: e2 e4\n";
    std::cout << "Quantum split: split b1 a3 c3\n";
    std::cout << "Exit: quit\n";

    while (true) {
        std::cout << game.board().toString();

        const std::string quantumInformation =
            game.quantumSummary();

        if (!quantumInformation.empty()) {
            std::cout
                << "\nQuantum positions:\n"
                << quantumInformation;
        }

        const quantum_chess::PieceColor turn =
            game.currentTurn();

        const bool whiteTurn =
            turn ==
            quantum_chess::PieceColor::White;

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
            << " command: ";

        std::string line;

        if (!std::getline(std::cin >> std::ws, line)) {
            break;
        }

        if (line == "quit" || line == "exit") {
            break;
        }

        std::istringstream input(line);
        std::vector<std::string> tokens;
        std::string token;

        while (input >> token) {
            tokens.push_back(token);
        }

        std::string errorMessage;
        bool successful = false;

        if (tokens.size() == 2) {
            successful = game.makeMove(
                tokens[0],
                tokens[1],
                errorMessage
            );
        } else if (
            tokens.size() == 4 &&
            tokens[0] == "split"
        ) {
            successful = game.splitKnight(
                tokens[1],
                tokens[2],
                tokens[3],
                errorMessage
            );
        } else {
            errorMessage =
                "Use e2 e4 or split b1 a3 c3.";
        }

        if (!successful) {
            std::cout
                << "\nInvalid command: "
                << errorMessage
                << "\n";
        }
    }

    std::cout << "\nGame ended.\n";
    return 0;
}
