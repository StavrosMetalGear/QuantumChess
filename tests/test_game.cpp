#include <cstdlib>
#include <iostream>
#include <string>

#include "quantum_chess/Game.h"

namespace {

void require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::cout << "PASSED: " << message << '\n';
}

bool move(
    quantum_chess::Game& game,
    const std::string& source,
    const std::string& destination
) {
    std::string error;
    return game.makeMove(source, destination, error);
}

void testInitialTurn() {
    quantum_chess::Game game;

    require(
        game.currentTurn() ==
            quantum_chess::PieceColor::White,
        "White moves first"
    );
}

void testValidPawnMoves() {
    quantum_chess::Game game;

    require(
        move(game, "e2", "e4"),
        "White pawn can move two squares initially"
    );

    require(
        move(game, "e7", "e5"),
        "Black pawn can move two squares initially"
    );
}

void testIllegalPawnMove() {
    quantum_chess::Game game;
    std::string error;

    require(
        !game.makeMove("e2", "e5", error),
        "Pawn cannot move three squares"
    );

    require(
        game.currentTurn() ==
            quantum_chess::PieceColor::White,
        "Turn does not change after an illegal move"
    );
}

void testKnightMovement() {
    quantum_chess::Game game;

    require(
        move(game, "g1", "f3"),
        "Knight can move in an L shape"
    );
}

void testBlockedBishop() {
    quantum_chess::Game game;
    std::string error;

    require(
        !game.makeMove("f1", "b5", error),
        "Bishop cannot move through its own pawn"
    );
}

void testFoolsMate() {
    quantum_chess::Game game;

    require(move(game, "f2", "f3"), "Fool's Mate move 1");
    require(move(game, "e7", "e5"), "Fool's Mate move 2");
    require(move(game, "g2", "g4"), "Fool's Mate move 3");
    require(move(game, "d8", "h4"), "Fool's Mate move 4");

    require(
        game.isCheckmate(
            quantum_chess::PieceColor::White
        ),
        "Fool's Mate is detected as checkmate"
    );
}

}

int main() {
    testInitialTurn();
    testValidPawnMoves();
    testIllegalPawnMove();
    testKnightMovement();
    testBlockedBishop();
    testFoolsMate();

    std::cout << "\nAll Quantum Chess tests passed.\n";
    return EXIT_SUCCESS;
}
