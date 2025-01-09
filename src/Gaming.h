#ifndef GAMING_H
#define GAMING_H

///@author Shane-Xue

#include <string>
#include <utility>
#include <vector>
#include "Board.h"

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
  enum class ViolationPolicy{
    Strict,
    Remind,
    Off
  };
  const size_t standardSize = 15;

  enum Direction : int
  {
    Horizontal = 0,
    Vertical = 1,
    Diagonal = 2,
    AntiDiagonal = 3
  };

  class Gaming
  {
  private:
    using GoBoard = Board<PieceType>;
    using Shift = std::pair<int, int>;
    GoBoard board;
    std::string senteName, goteName;
    EngineStatus engine;
    int moveCount = 1;
    ViolationPolicy violationPolicy = ViolationPolicy::Strict;

    std::vector<Shift> directions{
        {1, 0}, // Horizontal
        {0, 1}, // Vertical
        {1, 1}, // Diagonal
        {1, -1} // Anti-diagonal
    };

    /// @brief Counts the number of pieces in a direction starting from (x, y). Counts goes both ways.
    /// @param x
    /// @param y
    /// @param d
    /// @return Pieces count
    int directionCount(size_t x, size_t y, Direction d) const
    {
      PieceType current = board.at(x, y);
      int count = 0;
      int Xnow = x, Ynow = y;

      while (board.validateCoords(Xnow, Ynow) and board.at(Xnow, Ynow) == current)
      {
        count++;
        Xnow += directions[d].first;
        Ynow += directions[d].second;
      }

      Xnow = x - directions[d].first;
      Ynow = y - directions[d].second;

      while (board.validateCoords(Xnow, Ynow) && board.at(Xnow, Ynow) == current)
      {
        count++;
        Xnow -= directions[d].first;
        Ynow -= directions[d].second;
      }

      return count;
    }

    /// @brief Function Returning the degree of openness of a line of stones.
    /// @param row
    /// @param col
    /// @param dir
    /// @return Degree of openness.
    ///         0 = closed
    ///         1 = half open
    ///         2 = open
    /// @note This function is made private since I only want the bad move detection to use it when flagging the open-3s and open-4s.
    int openDegree(int row, int col, Direction dir)
    {
      PieceType current = board.at(row, col);
      if (current == PieceType::None)
        return 0;
      int openity = 0;

      int rptr = row, cptr = col;
      while (board.validateCoords(rptr, cptr) && board.at(rptr, cptr) == current)
      {
        rptr += directions[dir].first;
        cptr += directions[dir].second;
      }
      openity += static_cast<int>(board.validateCoords(rptr, cptr) and board.at(rptr, cptr) == PieceType::None);

      while (board.validateCoords(rptr, cptr) && board.at(rptr, cptr) == current)
      {
        rptr -= directions[dir].first;
        cptr -= directions[dir].second;
      }
      openity += static_cast<int>(board.validateCoords(rptr, cptr) and board.at(rptr, cptr) == PieceType::None);

      return openity;
    }

    /// @brief Check if there is a genuine five.
    /// @param row 
    /// @param col 
    /// @return Whether a genuine 5 exists in either direction of this position.
    /// @note This function exists since there is a overrule law in the determination of rule violations.
    /// I.E. when a violation and a 5 exist together, one side still wins.
    bool hasFive(int row, int col) const
    {
      if (board.at(row, col) == PieceType::None)
        return false;

      for (size_t i = 0; i < 4; i++)
      {
        if (directionCount(row, col, static_cast<Direction>(i)) == 5)
        {
          return true;
        }
      }
      return false;
    }

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

    void setViolationPolicy(ViolationPolicy policy)
    {
      violationPolicy = policy;
    }

    void setSenteName(const std::string &name)
    {
      senteName = name;
    }

    void setGoteName(const std::string &name)
    {
      goteName = name;
    }

    void setEngineStatus(EngineStatus status)
    {
      engine = status;
    }

    void setNames(const std::string &sente, const std::string &gote)
    {
      senteName = sente;
      goteName = gote;
    }

    

    /// @brief Max number of connected stones in all four directions
    /// @param row
    /// @param col
    /// @return Stone number
    int maxConnect(int row, int col) const
    {
      int m = 0;
      for (size_t dir = 0; dir < 4; dir++)
      {
        m = std::max(m, directionCount(row, col, static_cast<Direction>(dir)));
      }
      return m;
    }

    /// @brief Check if the game is over.
    /// @return Side that has won.
    PieceType checkWinFull() const
    {
      for (size_t row = 0; row < board.row_count(); row++)
      {
        for (size_t col = 0; col < board.col_count(); col++)
        {
          PieceType current = board.at(row, col);
          if (current == PieceType::None)
            continue;
          int currentCount = maxConnect(row, col);
          if (current == PieceType::Sente and currentCount == 5)
            return current;
          if (current == PieceType::Sente and currentCount > 5)
            return ((hasFive(row, col)) ? (PieceType::Sente) : (PieceType::Gote));
          if (current == PieceType::Gote and currentCount >= 5)
            return current;
        }
      }
      return PieceType::None;
    }

    /// @brief Check if anyone has won
    /// @param row
    /// @param col
    /// @return Side that has won. PieceType::None if no winner
    PieceType checkCurrentWin(int row, int col) const
    {
      
      if (board.at(row, col) == PieceType::None)
        return PieceType::None;
        int currentCount = maxConnect(row, col);
      if (board.at(row, col) == PieceType::Sente and currentCount == 5)
        return PieceType::Sente;
      if (board.at(row, col) == PieceType::Sente and currentCount > 5)
        // In this case we must also check if there are any legit 5s, ie, when a forbidden hand and a win simutaneously appear the win trumps.
        return ((hasFive(row, col)) ? (PieceType::Sente) : (PieceType::Gote));
      if (board.at(row, col) == PieceType::Gote and currentCount >= 5)
        return PieceType::Gote;
    }

    bool willBe(int row, int col, PieceType tp, int length)
    {
      if (board.at(row, col) != PieceType::None)
        return false;

      board.set(row, col, tp);
      int m = maxConnect(row, col);
      board.set(row, col, PieceType::None);

      return (m == length);
    }

    bool willFive(int row, int col, PieceType tp)
    {
      return willBe(row, col, tp, 5);
    }

    bool willFour(int row, int col, PieceType tp)
    {
      return willBe(row, col, tp, 4);
    }

    bool willThree(int row, int col, PieceType tp)
    {
      return willBe(row, col, tp, 3);
    }

    bool willBe(int row, int col, PieceType tp, int length, Direction dir)
    {
      if (board.at(row, col) != PieceType::None)
        return false;

      board.set(row, col, tp);
      int m = directionCount(row, col, dir);
      board.set(row, col, PieceType::None);

      return (m == length);
    }

    bool willLong(int row, int col, PieceType tp)
    {
      if (board.at(row, col) != PieceType::None)
        return false;

      board.set(row, col, tp);
      int m = maxConnect(row, col);
      board.set(row, col, PieceType::None);

      return (m > 5);
    }
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