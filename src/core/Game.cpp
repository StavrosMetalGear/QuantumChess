#include "quantum_chess/Game.h"

#include <cctype>

namespace quantum_chess {

Game::Game() = default;

const Board& Game::board() const {
    return board_;
}

PieceColor Game::currentTurn() const {
    return currentTurn_;
}

bool Game::parseSquare(
    const std::string& square,
    int& row,
    int& column
) {
    if (square.size() != 2) {
        return false;
    }

    const char file = static_cast<char>(
        std::tolower(static_cast<unsigned char>(square[0]))
    );
    const char rank = square[1];

    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
        return false;
    }

    column = file - 'a';
    row = 8 - (rank - '0');

    return true;
}

bool Game::makeMove(
    const std::string& source,
    const std::string& destination,
    std::string& errorMessage
) {
    int sourceRow = 0;
    int sourceColumn = 0;
    int destinationRow = 0;
    int destinationColumn = 0;

    if (!parseSquare(source, sourceRow, sourceColumn) ||
        !parseSquare(destination, destinationRow, destinationColumn)) {
        errorMessage = "Use squares such as e2 e4.";
        return false;
    }

    if (sourceRow == destinationRow &&
        sourceColumn == destinationColumn) {
        errorMessage = "Source and destination are the same.";
        return false;
    }

    const Piece& sourcePiece = board_.at(sourceRow, sourceColumn);
    const Piece& destinationPiece =
        board_.at(destinationRow, destinationColumn);

    if (sourcePiece.type == PieceType::None) {
        errorMessage = "There is no piece on the source square.";
        return false;
    }

    if (sourcePiece.color != currentTurn_) {
        errorMessage = "That piece belongs to the other player.";
        return false;
    }

    if (destinationPiece.color == currentTurn_) {
        errorMessage = "You cannot capture your own piece.";
        return false;
    }

    board_.movePiece(
        sourceRow,
        sourceColumn,
        destinationRow,
        destinationColumn
    );

    switchTurn();
    errorMessage.clear();

    return true;
}

void Game::switchTurn() {
    currentTurn_ =
        currentTurn_ == PieceColor::White
            ? PieceColor::Black
            : PieceColor::White;
}

}
