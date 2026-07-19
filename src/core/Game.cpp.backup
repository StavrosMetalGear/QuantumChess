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

    if (file < 'a' || file > 'h' ||
        rank < '1' || rank > '8') {
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
        !parseSquare(
            destination,
            destinationRow,
            destinationColumn)) {
        errorMessage = "Use squares such as e2 e4.";
        return false;
    }

    if (sourceRow == destinationRow &&
        sourceColumn == destinationColumn) {
        errorMessage =
            "Source and destination are the same.";
        return false;
    }

    const Piece sourcePiece =
        board_.at(sourceRow, sourceColumn);

    const Piece destinationPiece =
        board_.at(destinationRow, destinationColumn);

    if (sourcePiece.type == PieceType::None) {
        errorMessage =
            "There is no piece on the source square.";
        return false;
    }

    if (sourcePiece.color != currentTurn_) {
        errorMessage =
            "That piece belongs to the other player.";
        return false;
    }

    if (destinationPiece.color == currentTurn_) {
        errorMessage =
            "You cannot capture your own piece.";
        return false;
    }

    if (destinationPiece.type == PieceType::King) {
        errorMessage =
            "The king cannot be captured. It must be checkmated.";
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

    const Board previousBoard = board_;

    board_.movePiece(
        sourceRow,
        sourceColumn,
        destinationRow,
        destinationColumn
    );

    if (isInCheck(currentTurn_)) {
        board_ = previousBoard;
        errorMessage =
            "That move leaves your king in check.";
        return false;
    }

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
    const Piece& piece =
        board_.at(sourceRow, sourceColumn);

    const int rowDifference =
        destinationRow - sourceRow;

    const int columnDifference =
        destinationColumn - sourceColumn;

    const int absoluteRowDifference =
        std::abs(rowDifference);

    const int absoluteColumnDifference =
        std::abs(columnDifference);

    switch (piece.type) {
        case PieceType::Pawn:
            return isLegalPawnMove(
                sourceRow,
                sourceColumn,
                destinationRow,
                destinationColumn,
                errorMessage
            );

        case PieceType::Knight:
            if ((absoluteRowDifference == 2 &&
                 absoluteColumnDifference == 1) ||
                (absoluteRowDifference == 1 &&
                 absoluteColumnDifference == 2)) {
                return true;
            }

            errorMessage =
                "A knight must move in an L shape.";
            return false;

        case PieceType::Bishop:
            if (absoluteRowDifference !=
                absoluteColumnDifference) {
                errorMessage =
                    "A bishop must move diagonally.";
                return false;
            }

            if (!board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    destinationRow,
                    destinationColumn)) {
                errorMessage =
                    "The bishop's path is blocked.";
                return false;
            }

            return true;

        case PieceType::Rook:
            if (rowDifference != 0 &&
                columnDifference != 0) {
                errorMessage =
                    "A rook must move horizontally or vertically.";
                return false;
            }

            if (!board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    destinationRow,
                    destinationColumn)) {
                errorMessage =
                    "The rook's path is blocked.";
                return false;
            }

            return true;

        case PieceType::Queen:
            if (rowDifference != 0 &&
                columnDifference != 0 &&
                absoluteRowDifference !=
                    absoluteColumnDifference) {
                errorMessage =
                    "A queen must move straight or diagonally.";
                return false;
            }

            if (!board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    destinationRow,
                    destinationColumn)) {
                errorMessage =
                    "The queen's path is blocked.";
                return false;
            }

            return true;

        case PieceType::King:
            if (absoluteRowDifference <= 1 &&
                absoluteColumnDifference <= 1) {
                return true;
            }

            errorMessage =
                "A king can move only one square.";
            return false;

        case PieceType::None:
            errorMessage =
                "There is no piece on the source square.";
            return false;
    }

    errorMessage = "Unknown piece type.";
    return false;
}

bool Game::isLegalPawnMove(
    int sourceRow,
    int sourceColumn,
    int destinationRow,
    int destinationColumn,
    std::string& errorMessage
) const {
    const Piece& pawn =
        board_.at(sourceRow, sourceColumn);

    const Piece& destination =
        board_.at(destinationRow, destinationColumn);

    const int direction =
        pawn.color == PieceColor::White ? -1 : 1;

    const int startingRow =
        pawn.color == PieceColor::White ? 6 : 1;

    const int rowDifference =
        destinationRow - sourceRow;

    const int columnDifference =
        destinationColumn - sourceColumn;

    if (columnDifference == 0) {
        if (destination.type != PieceType::None) {
            errorMessage =
                "A pawn cannot move forward into an occupied square.";
            return false;
        }

        if (rowDifference == direction) {
            return true;
        }

        if (sourceRow == startingRow &&
            rowDifference == 2 * direction) {
            const int middleRow =
                sourceRow + direction;

            if (board_.at(
                    middleRow,
                    sourceColumn).type ==
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
            errorMessage =
                "A pawn can move diagonally only when capturing.";
            return false;
        }

        return true;
    }

    errorMessage = "Illegal pawn movement.";
    return false;
}

bool Game::pieceAttacksSquare(
    int sourceRow,
    int sourceColumn,
    int targetRow,
    int targetColumn
) const {
    const Piece& piece =
        board_.at(sourceRow, sourceColumn);

    if (piece.type == PieceType::None) {
        return false;
    }

    const int rowDifference =
        targetRow - sourceRow;

    const int columnDifference =
        targetColumn - sourceColumn;

    const int absoluteRowDifference =
        std::abs(rowDifference);

    const int absoluteColumnDifference =
        std::abs(columnDifference);

    switch (piece.type) {
        case PieceType::Pawn: {
            const int direction =
                piece.color == PieceColor::White
                    ? -1
                    : 1;

            return rowDifference == direction &&
                   absoluteColumnDifference == 1;
        }

        case PieceType::Knight:
            return
                (absoluteRowDifference == 2 &&
                 absoluteColumnDifference == 1) ||
                (absoluteRowDifference == 1 &&
                 absoluteColumnDifference == 2);

        case PieceType::Bishop:
            return
                absoluteRowDifference ==
                    absoluteColumnDifference &&
                board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    targetRow,
                    targetColumn
                );

        case PieceType::Rook:
            return
                (rowDifference == 0 ||
                 columnDifference == 0) &&
                board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    targetRow,
                    targetColumn
                );

        case PieceType::Queen:
            return
                (rowDifference == 0 ||
                 columnDifference == 0 ||
                 absoluteRowDifference ==
                    absoluteColumnDifference) &&
                board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    targetRow,
                    targetColumn
                );

        case PieceType::King:
            return
                absoluteRowDifference <= 1 &&
                absoluteColumnDifference <= 1;

        case PieceType::None:
            return false;
    }

    return false;
}

bool Game::isInCheck(PieceColor color) const {
    int kingRow = -1;
    int kingColumn = -1;

    for (int row = 0; row < 8; ++row) {
        for (int column = 0; column < 8; ++column) {
            const Piece& piece =
                board_.at(row, column);

            if (piece.type == PieceType::King &&
                piece.color == color) {
                kingRow = row;
                kingColumn = column;
            }
        }
    }

    if (kingRow == -1 || kingColumn == -1) {
        return false;
    }

    const PieceColor opponent =
        color == PieceColor::White
            ? PieceColor::Black
            : PieceColor::White;

    for (int row = 0; row < 8; ++row) {
        for (int column = 0; column < 8; ++column) {
            const Piece& piece =
                board_.at(row, column);

            if (piece.color == opponent &&
                pieceAttacksSquare(
                    row,
                    column,
                    kingRow,
                    kingColumn)) {
                return true;
            }
        }
    }

    return false;
}


bool Game::moveLeavesKingInCheck(
    int sourceRow,
    int sourceColumn,
    int destinationRow,
    int destinationColumn,
    PieceColor color
) const {
    Game testGame = *this;

    testGame.board_.movePiece(
        sourceRow,
        sourceColumn,
        destinationRow,
        destinationColumn
    );

    return testGame.isInCheck(color);
}

bool Game::hasAnyLegalMove(PieceColor color) const {
    for (int sourceRow = 0; sourceRow < 8; ++sourceRow) {
        for (int sourceColumn = 0; sourceColumn < 8; ++sourceColumn) {
            const Piece& piece =
                board_.at(sourceRow, sourceColumn);

            if (piece.color != color) {
                continue;
            }

            for (int destinationRow = 0;
                 destinationRow < 8;
                 ++destinationRow) {
                for (int destinationColumn = 0;
                     destinationColumn < 8;
                     ++destinationColumn) {

                    if (sourceRow == destinationRow &&
                        sourceColumn == destinationColumn) {
                        continue;
                    }

                    const Piece& destination =
                        board_.at(
                            destinationRow,
                            destinationColumn
                        );

                    if (destination.color == color) {
                        continue;
                    }

                    if (destination.type == PieceType::King) {
                        continue;
                    }

                    std::string errorMessage;

                    if (!isLegalMove(
                            sourceRow,
                            sourceColumn,
                            destinationRow,
                            destinationColumn,
                            errorMessage)) {
                        continue;
                    }

                    if (!moveLeavesKingInCheck(
                            sourceRow,
                            sourceColumn,
                            destinationRow,
                            destinationColumn,
                            color)) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool Game::isCheckmate(PieceColor color) const {
    return isInCheck(color) &&
           !hasAnyLegalMove(color);
}

bool Game::isStalemate(PieceColor color) const {
    return !isInCheck(color) &&
           !hasAnyLegalMove(color);
}

void Game::switchTurn() {
    currentTurn_ =
        currentTurn_ == PieceColor::White
            ? PieceColor::Black
            : PieceColor::White;
}

}
