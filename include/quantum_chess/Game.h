#pragma once

#include "quantum_chess/Board.h"

namespace quantum_chess {

class Game {
public:
    Game();

    const Board& board() const;

private:
    Board board_;
};

}
