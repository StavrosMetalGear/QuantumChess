#pragma once

namespace quantum_chess {

enum class PieceType {
    None,
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn
};

enum class PieceColor {
    None,
    White,
    Black
};

struct Piece {
    PieceType type = PieceType::None;
    PieceColor color = PieceColor::None;
    bool hasMoved = false;

    bool isQuantum = false;
    int quantumGroupId = -1;
    double probability = 1.0;
};

}
