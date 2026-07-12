#include "quantum_chess/Board.h"

#include <sstream>

namespace quantum_chess {

Board::Board() {
    reset();
}

void Board::reset() {
    for (auto& row : squares_) {
        for (auto& square : row) {
            square = {};
        }
    }

    const std::array<PieceType, 8> backRank = {
        PieceType::Rook,
        PieceType::Knight,
        PieceType::Bishop,
        PieceType::Queen,
        PieceType::King,
        PieceType::Bishop,
        PieceType::Knight,
        PieceType::Rook
    };

    for (int column = 0; column < 8; ++column) {
        squares_[0][column] = {backRank[column], PieceColor::Black};
        squares_[1][column] = {PieceType::Pawn, PieceColor::Black};

        squares_[6][column] = {PieceType::Pawn, PieceColor::White};
        squares_[7][column] = {backRank[column], PieceColor::White};
    }
}

char Board::pieceToChar(const Piece& piece) {
    char symbol = '.';

    switch (piece.type) {
        case PieceType::King:   symbol = 'k'; break;
        case PieceType::Queen:  symbol = 'q'; break;
        case PieceType::Rook:   symbol = 'r'; break;
        case PieceType::Bishop: symbol = 'b'; break;
        case PieceType::Knight: symbol = 'n'; break;
        case PieceType::Pawn:   symbol = 'p'; break;
        case PieceType::None:   return '.';
    }

    if (piece.color == PieceColor::White) {
        symbol = static_cast<char>(symbol - 'a' + 'A');
    }

    return symbol;
}

std::string Board::toString() const {
    std::ostringstream output;

    output << "\n    a b c d e f g h\n";
    output << "  +-----------------+\n";

    for (int row = 0; row < 8; ++row) {
        const int rank = 8 - row;

        output << rank << " | ";

        for (int column = 0; column < 8; ++column) {
            output << pieceToChar(squares_[row][column]) << ' ';
        }

        output << "| " << rank << '\n';
    }

    output << "  +-----------------+\n";
    output << "    a b c d e f g h\n";

    return output.str();
}

}
