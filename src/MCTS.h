#ifndef MCTS_H
#define MCTS_H

#include <random>
#include <vector>
#include <utility>
#include "Gaming.h"

namespace GosFrontline {

class MCTS {
private:
    std::random_device rd;
    std::mt19937 gen;

public:
    MCTS() : gen(rd()) {}

    // Generate a random valid move for the given game state
    std::pair<int, int> getRandomMove(const Gaming& game) {
        std::vector<std::pair<int, int>> validMoves;
        
        // Collect all valid moves
        for(size_t i = 0; i < game.board.row_count(); i++) {
            for(size_t j = 0; j < game.board.col_count(); j++) {
                if(game.isEmpty(i, j)) {
                    validMoves.push_back({i, j});
                }
            }
        }

        if(validMoves.empty()) {
            return {-1, -1}; // No valid moves available
        }

        // Generate random index
        std::uniform_int_distribution<> dis(0, validMoves.size() - 1);
        int randomIndex = dis(gen);

        return validMoves[randomIndex];
    }
};

} // namespace GosFrontline

#endif // MCTS_H