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

private:
    std::array<std::array<Piece, 8>, 8> squares_{};

    static char pieceToChar(const Piece& piece);
};

}
