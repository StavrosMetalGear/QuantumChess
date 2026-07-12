#include "quantum_chess/Game.h"

namespace quantum_chess {

Game::Game() = default;

const Board& Game::board() const {
    return board_;
}

}
