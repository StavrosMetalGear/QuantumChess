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

private:
    Board board_;
    PieceColor currentTurn_ = PieceColor::White;

    static bool parseSquare(
        const std::string& square,
        int& row,
        int& column
    );

    void switchTurn();
};

}
