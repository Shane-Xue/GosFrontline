#ifndef GAMING_H
#define GAMING_H

///@author Shane-Xue

#include <cassert>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <stdexcept>
#include "Board.h"

namespace GosFrontline
{
  enum class PieceType : int
  {
    None = 0,
    Sente = 1,
    Gote = 2
  };
  enum class ViolationPolicy
  {
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
    using Move = std::tuple<int, int, PieceType>;
    GoBoard board;
    std::string senteName, goteName;
    PieceType engine;
    int moveCount = 0; // Number of moves made, not current move number
    ViolationPolicy violationPolicy = ViolationPolicy::Strict;
    std::vector<Move> moves{};
    friend class MCTS;

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

#ifdef DEBUG
      std::cout << count;
#endif

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
    int openDegree(int row, int col, Direction dir) const
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

    /// @brief Log move onto the move log
    /// @throws std::invalid_argument when coordinates are invalid.
    /// @param row
    /// @param col
    /// @note Please check if row and col are valid *outside* of the function.
    ///       Function was declared as private to prevent record hacking.
    void recordMove(int row, int col)
    {
      if (not board.validateCoords(row, col))
      {
        throw std::invalid_argument("Invalid coordinates");
      }

      moves.push_back(std::make_tuple(row, col, toMove()));
      moveCount++;
    }

    bool _undo_last()
    {
      if (moveCount == 0)
      {
        return false;
      }
      Move last = moves.back();
      if (not(board.at(std::get<0>(last), std::get<1>(last)) == std::get<2>(last)))
        throw std::runtime_error("Move log is corrupted");
      moves.pop_back();
      board.set(std::get<0>(last), std::get<1>(last), PieceType::None);
      moveCount--;
      return true;
    }

  public:
    Gaming();

    // Initialize with board dimensions and player names
    Gaming(int rows, int cols, const std::string &sente, const std::string &gote, PieceType engineStat = PieceType::Gote)
    {
      board = GoBoard(rows, cols, PieceType::None);
      senteName = sente;
      goteName = gote;
      engine = engineStat;
    }

    // Initialize with board dimensions and engine status
    Gaming(int rows, int cols, PieceType engineStat)
    {
      board = GoBoard(rows, cols, PieceType::None);
      senteName = "Anonymous";
      goteName = "Anonymous";
      engine = engineStat;
    }

    // Initialize with player names and engine status
    Gaming(const std::string &sente, const std::string &gote, PieceType engineStat = PieceType::Gote)
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
      engine = PieceType::Gote; // Default engine status
    }

    /// @brief This does what its name says.
    /// @param policy
    void setViolationPolicy(ViolationPolicy policy)
    {
      violationPolicy = policy;
    }

    /// @brief This does what its name says.
    /// @param name
    void setSenteName(const std::string &name)
    {
      senteName = name;
    }

    /// @brief This does what its name says.
    /// @param name
    void setGoteName(const std::string &name)
    {
      goteName = name;
    }

    /// @brief This does what its name says.
    /// @param status
    void setEngineStatus(PieceType status)
    {
      engine = status;
    }

    /// @brief This does what its name says.
    /// @param sente
    /// @param gote
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
      return PieceType::None;
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

    int countLength(int row, int col, int length) const
    {
      if (board.at(row, col) == PieceType::None)
      {
        return 0;
      }

      int count = 0;
      for (size_t i = 0; i < 4; i++)
      {
        count += static_cast<int>(directionCount(row, col, static_cast<Direction>(i)) == length);
      }
      return count;
    }

    int countThree(int row, int col) const
    {
      return countLength(row, col, 3);
    }

    int countFour(int row, int col) const
    {
      return countLength(row, col, 4);
    }

    int countLiveThree(int row, int col) const
    {
      if (board.at(row, col) == PieceType::None)
      {
        return 0;
      }

      int count = 0;
      for (size_t i = 0; i < 4; i++)
      {
        count += static_cast<int>((directionCount(row, col, static_cast<Direction>(i)) == 3) and openDegree(row, col, static_cast<Direction>(i)) == 2);
      }
      return count;
    }

    bool isLong(int row, int col) const
    {
      if (hasFive(row, col))
        return false;

      for (size_t i = 0; i < 4; i++)
      {
        if (directionCount(row, col, static_cast<Direction>(i)) > 5)
        {
          return true;
        }
      }
      return false;
    }

    bool violationAt(int row, int col)
    {
      if (board.at(row, col) == PieceType::Gote)
      {
        return false; // Gote can't make a violation.
      }

      if (board.at(row, col) != PieceType::None) // if it is already placed
        return (countLiveThree(row, col) > 1) or // 33 and 334
               (countFour(row, col) >= 2) or     // 44 and 344
               (isLong(row, col));

      else
      {
        board.set(row, col, PieceType::Sente);
        bool result = violationAt(row, col);
        board.set(row, col, PieceType::None);
        return result;
      } // if nothing is placed yet
    }

    bool violation(int row, int col)
    {
      if (toMove() == PieceType::Gote)
      {
        return false;
      }

      bool empty_flag = false;
      if (not(board.at(row, col) == PieceType::Sente))
      {
        board.set(row, col, PieceType::Sente);
        empty_flag = true;
      }

      if (violationAt(row, col))
      {
        if (empty_flag)
        {
          board.set(row, col, PieceType::None);
        }
        return true;
      }

      // Must search all stones connected since this one move may cause a change in states of all stones connected
      for (const auto &dir : directions)
      {
        int x = row + dir.first, y = col + dir.second;

        // Traverse in the positive direction
        while (board.validateCoords(x, y) && board.at(x, y) == PieceType::Sente)
        {
          if (violationAt(x, y))
          {
            if (empty_flag)
            {
              board.set(row, col, PieceType::None);
            }
            return true;
          }
          x += dir.first;
          y += dir.second;
        }

        // Traverse in the negative direction
        x = row - dir.first;
        y = col - dir.second;
        while (board.validateCoords(x, y) && board.at(x, y) == PieceType::Sente)
        {
          if (violationAt(x, y))
          {
            if (empty_flag)
            {
              board.set(row, col, PieceType::None);
            }
            return true;
          }
          x -= dir.first;
          y -= dir.second;
        }
      }
      if (empty_flag)
      {
        board.set(row, col, PieceType::None);
      }
      return false;
    }

    PieceType toMove() const
    {
      return static_cast<PieceType>(moveCount % 2 + 1);
    }

    /// @brief Make a move.
    /// @param row
    /// @param col
    /// @return If the move was successfully made
    /// @note This API is solely designated for a human player.
    ///       If it is an algorithm, use protected function makeMoveEngine(row, col) instead.
    bool makeMove(int row, int col)
    {
      if ((not board.validateCoords(row, col)) or board.at(row, col) != PieceType::None)
      {
        return false;
      }

      if (static_cast<int>(toMove()) == static_cast<int>(engine))
      {
        throw std::runtime_error("It is not your turn!");
      }

      if ((toMove() == PieceType::Sente) and violation(row, col))
      {
        return false;
      }

      board.set(row, col, toMove());
      recordMove(row, col);
      return true;
    }

    bool undo()
    {
      if (static_cast<int>(toMove()) == static_cast<int>(engine))
      {
        throw std::runtime_error("Engine seems to be too slow, or is undoing.");
      }
      if (engine == PieceType::None)
      {
        return _undo_last();
      }
      return (_undo_last() and _undo_last());
    }

    const std::vector<std::vector<PieceType>> &getBoard() const
    {
      return board.get_board();
    }

    const std::vector<Move> &getSequence() const
    {
      return moves;
    }

    std::string getSenteName() const{
      return senteName;
    }

    std::string getGoteName() const{
      return goteName;
    }

    int movesMade() const
    {
      return moveCount;
    }

    bool isEmpty(int row, int col) const
    {
      return board.at(row, col) == PieceType::None;
    }

    PieceType engineSide()
    {
      return engine;
    }

    bool makeMoveEngine(int row, int col)
    {
      if ((not board.validateCoords(row, col)) or board.at(row, col) != PieceType::None)
      {
        throw std::runtime_error("Invalid move from engine.");
      }

      if (static_cast<int>(toMove()) != static_cast<int>(engine))
      {
        throw std::runtime_error("Engine made move on wrong turn.");
      }

      if ((toMove() == PieceType::Sente) and violation(row, col))
      {
        throw std::runtime_error("Engine made a violation move.");
      }

      board.set(row, col, toMove());
      recordMove(row, col);
      return true;
    }

    bool isValidCoord(int row, int col)
    {
      return board.validateCoords(row, col);
    }

    void clearBoard()
    {
      board = GoBoard(board.row_count(), board.col_count(), PieceType::None);
      moves.clear();
      moveCount = 0;
    }

    void clearBoard(int row, int col)
    {
      board = GoBoard(row, col, PieceType::None);
      moves.clear();
      moveCount = 0;
    }
  };

  /// @brief Default Constructor. 15x15 board with Engine playing Gote.
  Gaming::Gaming()
  {
    board = GoBoard(standardSize, standardSize, PieceType::None);
    senteName = "Anonymous";
    goteName = "Gryffin Engine";
    engine = PieceType::Gote;
  }

  // Gaming::~Gaming()
  // {
  // }

  PieceType Opposite(PieceType piece)
  {
    if (piece == PieceType::None)
    {
      return piece;
    }
    if ((piece == PieceType::Sente))
    {
      return PieceType::Gote;
    }
    return PieceType::Sente;
  }

} // namespace GosFrontline

#endif // GAMING_H