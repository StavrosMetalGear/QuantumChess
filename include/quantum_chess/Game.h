#pragma once

#include <string>

#include "quantum_chess/Board.h"

namespace quantum_chess {

class Game {
public:
    Game();

    const Board& board() const;
    PieceColor currentTurn() const;

    bool makeMove(
        const std::string& source,
        const std::string& destination,
        std::string& errorMessage
    );

    bool isInCheck(PieceColor color) const;
    bool hasAnyLegalMove(PieceColor color) const;
    bool isCheckmate(PieceColor color) const;
    bool isStalemate(PieceColor color) const;

private:
    Board board_;
    PieceColor currentTurn_ = PieceColor::White;

    bool enPassantAvailable_ = false;
    int enPassantTargetRow_ = -1;
    int enPassantTargetColumn_ = -1;
    int enPassantPawnRow_ = -1;
    int enPassantPawnColumn_ = -1;

    static bool parseSquare(
        const std::string& square,
        int& row,
        int& column
    );

    bool isLegalMove(
        int sourceRow,
        int sourceColumn,
        int destinationRow,
        int destinationColumn,
        std::string& errorMessage
    ) const;

    bool isLegalPawnMove(
        int sourceRow,
        int sourceColumn,
        int destinationRow,
        int destinationColumn,
        std::string& errorMessage
    ) const;

    bool pieceAttacksSquare(
        int sourceRow,
        int sourceColumn,
        int targetRow,
        int targetColumn
    ) const;

    bool isLegalCastlingMove(
        int sourceRow,
        int sourceColumn,
        int destinationRow,
        int destinationColumn,
        std::string& errorMessage
    ) const;

    void moveCastlingRook(
        int kingRow,
        int kingSourceColumn,
        int kingDestinationColumn
    );

    bool moveLeavesKingInCheck(
        int sourceRow,
        int sourceColumn,
        int destinationRow,
        int destinationColumn,
        PieceColor color
    ) const;

    void promotePawnIfNeeded(
        int row,
        int column
    );

    void switchTurn();
};

}
