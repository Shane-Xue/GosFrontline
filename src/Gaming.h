#ifndef GAMING_H
#define GAMING_H

#include <vector>
#include <utility>
#include "Board.h"
#include <string>

namespace GosFrontline
{
    enum class EngineStatus : int
    {
        Disabled = 0,
        Sente = 1,
        Gote = 2
    };
    enum class PieceType : int
    {
        None = 0,
        Sente = 1,
        Gote = 2
    };
    const size_t standardSize = 15;

    class Gaming
    {
    private:
        using GoBoard = Board<PieceType>;
        GoBoard board;
        std::string senteName, goteName;
        EngineStatus engine;
        int moveCount = 1;

    public:
        Gaming();

        // Initialize with board dimensions and player names
        Gaming(int rows, int cols, const std::string &sente, const std::string &gote, EngineStatus engineStat = EngineStatus::Gote)
        {
            board = GoBoard(rows, cols, PieceType::None);
            senteName = sente;
            goteName = gote;
            engine = engineStat;
        }

        // Initialize with board dimensions and engine status
        Gaming(int rows, int cols, EngineStatus engineStat)
        {
            board = GoBoard(rows, cols, PieceType::None);
            senteName = "Anonymous";
            goteName = "Anonymous";
            engine = engineStat;
        }

        // Initialize with player names and engine status
        Gaming(const std::string &sente, const std::string &gote, EngineStatus engineStat = EngineStatus::Gote)
        {
            board = GoBoard(standardSize, standardSize, PieceType::None); // Default board size
            senteName = sente;
            goteName = gote;
            engine = engineStat;
        }

        // Initialize with player names only
        Gaming(const std::string &sente, const std::string &gote)
        {
            board = GoBoard(standardSize, standardSize, PieceType::None); // Default board size
            senteName = sente;
            goteName = gote;
            engine = EngineStatus::Gote; // Default engine status
        }

        /// @brief Check if any side has won. Full board search.
        /// @note Time complexity O(n*m*k), where n,m are dimensions and k is winning length (5).
        /// @return The winning side, or PieceType::None if nobody has won.
        PieceType checkWinFull() const
        {
            if (moveCount < 9)
                return PieceType::None;

            const int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
            size_t rows = board.row_count();
            size_t cols = board.col_count();

            for (size_t i = 0; i < rows; i++)
            {
                for (size_t j = 0; j < cols; j++)
                {
                    PieceType current = board.at(i, j);
                    if (current == PieceType::None)
                        continue;

                    for (const auto &dir : directions)
                    {
                        int count = 1;
                        for (int step = 1; step < 6; step++)
                        {
                            int newRow = i + dir[0] * step;
                            int newCol = j + dir[1] * step;

                            if (newRow < 0 || newRow >= rows || newCol < 0 || newCol >= cols)
                                break;
                            if (board.at(newRow, newCol) != current)
                                break;
                            count++;
                        }
                        if ((current == PieceType::Sente && count == 5) ||
                            (current == PieceType::Gote && count >= 5))
                        {
                            return current;
                        }
                    }
                }
            }
            return PieceType::None;
        }

        /// @brief Check if last move at (row,col) created a winning sequence.
        /// @note Time complexity O(k) where k is winning length (5).
        /// @return The winning side, or PieceType::None if nobody has won.
        PieceType checkWinAt(size_t row, size_t col) const
        {
            if (moveCount < 9)
                return PieceType::None;

            const int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
            PieceType current = board.at(row, col);
            if (current == PieceType::None)
                return PieceType::None;

            for (const auto &dir : directions)
            {
                int count = 1;
                // Check in positive direction
                for (int step = 1; step < 6; step++)
                {
                    int newRow = row + dir[0] * step;
                    int newCol = col + dir[1] * step;
                    if (newRow < 0 || newRow >= board.row_count() ||
                        newCol < 0 || newCol >= board.col_count())
                        break;
                    if (board.at(newRow, newCol) != current)
                        break;
                    count++;
                }
                // Check in negative direction
                for (int step = 1; step < 6; step++)
                {
                    int newRow = row - dir[0] * step;
                    int newCol = col - dir[1] * step;
                    if (newRow < 0 || newRow >= board.row_count() ||
                        newCol < 0 || newCol >= board.col_count())
                        break;
                    if (board.at(newRow, newCol) != current)
                        break;
                    count++;
                }
                if ((current == PieceType::Sente && count == 5) ||
                    (current == PieceType::Gote && count >= 5))
                {
                    return current;
                }
            }
            return PieceType::None;
        }



        ~Gaming();
    };

    /// @brief Default Constructor. 15x15 board with Engine playing Gote.
    Gaming::Gaming()
    {
        board = GoBoard(standardSize, standardSize, PieceType::None);
        senteName = "Anonymous";
        goteName = "Gryffin Engine";
        engine = EngineStatus::Gote;
    }

    Gaming::~Gaming()
    {
    }

} // namespace GosFrontline

#endif // GAMING_H