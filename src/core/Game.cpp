#include "quantum_chess/Game.h"

#include <cctype>
#include <cstdlib>

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

    if (!isLegalMove(
            sourceRow,
            sourceColumn,
            destinationRow,
            destinationColumn,
            errorMessage)) {
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

bool Game::isLegalMove(
    int sourceRow,
    int sourceColumn,
    int destinationRow,
    int destinationColumn,
    std::string& errorMessage
) const {
    const Piece& piece = board_.at(sourceRow, sourceColumn);

    if (piece.type == PieceType::Pawn) {
        return isLegalPawnMove(
            sourceRow,
            sourceColumn,
            destinationRow,
            destinationColumn,
            errorMessage
        );
    }

    return true;
}

bool Game::isLegalPawnMove(
    int sourceRow,
    int sourceColumn,
    int destinationRow,
    int destinationColumn,
    std::string& errorMessage
) const {
    const Piece& pawn = board_.at(sourceRow, sourceColumn);
    const Piece& destination =
        board_.at(destinationRow, destinationColumn);

    const int direction =
        pawn.color == PieceColor::White ? -1 : 1;

    const int startingRow =
        pawn.color == PieceColor::White ? 6 : 1;

    const int rowDifference = destinationRow - sourceRow;
    const int columnDifference =
        destinationColumn - sourceColumn;

    if (columnDifference == 0) {
        if (destination.type != PieceType::None) {
            errorMessage = "A pawn cannot move forward into an occupied square.";
            return false;
        }

        if (rowDifference == direction) {
            return true;
        }

        if (sourceRow == startingRow &&
            rowDifference == 2 * direction) {
            const int middleRow = sourceRow + direction;

            if (board_.at(middleRow, sourceColumn).type ==
                PieceType::None) {
                return true;
            }

            errorMessage = "The pawn is blocked.";
            return false;
        }

        errorMessage = "Illegal pawn movement.";
        return false;
    }

    if (std::abs(columnDifference) == 1 &&
        rowDifference == direction) {
        if (destination.type == PieceType::None) {
            errorMessage = "A pawn can move diagonally only when capturing.";
            return false;
        }

        if (destination.color == pawn.color) {
            errorMessage = "A pawn cannot capture its own piece.";
            return false;
        }

        return true;
    }

    errorMessage = "Illegal pawn movement.";
    return false;
}

void Game::switchTurn() {
    currentTurn_ =
        currentTurn_ == PieceColor::White
            ? PieceColor::Black
            : PieceColor::White;
}

}
