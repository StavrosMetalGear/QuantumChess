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

void testPawnMovement() {
    quantum_chess::Game game;

    require(
        move(game, "e2", "e4"),
        "White pawn can move two squares"
    );

    require(
        move(game, "e7", "e5"),
        "Black pawn can move two squares"
    );
}

void testIllegalPawnMovement() {
    quantum_chess::Game game;
    std::string error;

    require(
        !game.makeMove("e2", "e5", error),
        "Pawn cannot move three squares"
    );
}

void testKnightSplit() {
    quantum_chess::Game game;

    std::string error;

    require(
        game.splitKnight(
            "b1",
            "a3",
            "c3",
            error
        ),
        "Knight can split into two legal squares"
    );

    const std::string summary =
        game.quantumSummary();

    require(
        summary.find("a3") != std::string::npos,
        "Quantum summary contains a3"
    );

    require(
        summary.find("c3") != std::string::npos,
        "Quantum summary contains c3"
    );

    require(
        summary.find("50%") != std::string::npos,
        "Quantum branches have 50 percent probability"
    );
}

void testIllegalQuantumSplit() {
    quantum_chess::Game game;

    std::string error;

    require(
        !game.splitKnight(
            "b1",
            "b3",
            "c3",
            error
        ),
        "Quantum split rejects an illegal knight destination"
    );
}

void testQuantumMeasurement() {
    quantum_chess::Game game;

    std::string error;

    require(
        game.splitKnight(
            "b1",
            "a3",
            "c3",
            error
        ),
        "Quantum knight split succeeds before measurement"
    );

    std::string result;

    require(
        game.measureQuantumPiece(
            "a3",
            result,
            error
        ),
        "Quantum group can be measured"
    );

    require(
        game.quantumSummary().empty(),
        "Measurement removes the quantum superposition"
    );

    require(
        result.find("collapsed to") !=
            std::string::npos,
        "Measurement reports the collapse result"
    );
}

void testQuantumTurnChange() {
    quantum_chess::Game game;

    std::string error;

    require(
        game.splitKnight(
            "b1",
            "a3",
            "c3",
            error
        ),
        "Quantum split succeeds"
    );

    require(
        game.currentTurn() ==
            quantum_chess::PieceColor::Black,
        "Quantum split consumes White's turn"
    );
}

void testFoolsMate() {
    quantum_chess::Game game;

    require(move(game, "f2", "f3"), "Fool's Mate 1");
    require(move(game, "e7", "e5"), "Fool's Mate 2");
    require(move(game, "g2", "g4"), "Fool's Mate 3");
    require(move(game, "d8", "h4"), "Fool's Mate 4");

    require(
        game.isCheckmate(
            quantum_chess::PieceColor::White
        ),
        "Checkmate detection still works"
    );
}

}

int main() {
    std::cout << "Quantum Chess automated tests\n\n";

    testInitialTurn();
    testPawnMovement();
    testIllegalPawnMovement();

    testKnightSplit();
    testIllegalQuantumSplit();
    testQuantumMeasurement();
    testQuantumTurnChange();

    testFoolsMate();

    std::cout
        << "\nAll Quantum Chess tests passed.\n";

    return EXIT_SUCCESS;
}
