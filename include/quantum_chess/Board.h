#pragma once

#include <array>
#include <string>

#include "quantum_chess/Piece.h"

namespace quantum_chess {

class Board {
public:
    Board();

    void reset();
    std::string toString() const;

    const Piece& at(int row, int column) const;
    Piece& at(int row, int column);

    void setPiece(
        int row,
        int column,
        const Piece& piece
    );

    void clearSquare(
        int row,
        int column
    );

    bool movePiece(
        int sourceRow,
        int sourceColumn,
        int destinationRow,
        int destinationColumn
    );

    bool isPathClear(
        int sourceRow,
        int sourceColumn,
        int destinationRow,
        int destinationColumn
    ) const;

private:
    std::array<std::array<Piece, 8>, 8> squares_{};

    static char pieceToChar(const Piece& piece);
};

}
