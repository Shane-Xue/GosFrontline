#ifndef BOARD_H
#define BOARD_H

/// @author Shane-Xue

#include <cassert>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace GosFrontline
{

  template <typename T>
  class Board
  {
  private:
    using Grid = std::vector<std::vector<T>>;
    Grid board;
    size_t col, row;

  public:
    // Initializers
    Board() {};

    Board(size_t r, size_t c, T init_val = T()) : board(std::vector<std::vector<T>>(r, std::vector<T>(c, init_val))),
                                                  col(c), row(r) {};

    // Copy and Move initializers and operator=
    Board(const Board &other) : board(other.board), col(other.col), row(other.row) {};

    Board operator=(const Board &other)
    {
      board = other.board;
      col = other.col;
      row = other.row;
      return *this;
    }

    Board(Board &&other) noexcept : col(other.col), row(other.row)
    {
      board = std::move(other.board);
      row = other.row;
      col = other.col;
      other.col = 0;
      other.row = 0;
    }

    Board operator=(Board &&other) noexcept
    {
      board = std::move(other.board);
      row = other.row;
      col = other.col;
      other.col = 0;
      other.row = 0;
      return *this;
    }

    // Accessing elements

    /// @brief take row, but disallow modification of vector
    /// @param index
    /// @return const vector<T>&
    const std::vector<T> &operator[](size_t index)
    {
      return board[index];
    }

    /// @brief access specific element that enables modifying
    /// @tparam IndexType
    /// @param indices
    /// @return non-const T&
    template <typename IndexType>
    T &operator[](std::pair<IndexType, IndexType> indices)
    {
      return board[indices.first][indices.second];
    }

    /// @brief const version of pair operator[]
    /// @tparam IndexType
    /// @param indices
    /// @return const T& value
    template <typename IndexType>
    const T &operator[](std::pair<IndexType, IndexType> indices)
    {
      return board[indices.first][indices.second];
    }

    /// @brief get element at @row_param @col_param
    /// @param row_param
    /// @param col_param
    /// @return element at this place
    const T &at(size_t row_param, size_t col_param) const
    {
      return board.at(row_param).at(col_param);
    }

    // /// @brief const version of at
    // /// @param row_param
    // /// @param col_param
    // /// @return const T& value
    // const T& at(size_t row_param, size_t col_param) const
    // {
    //   return board.at(row_param).at(col_param);
    // }

    /// @brief set element at row_param, col_param
    /// @param row_param
    /// @param col_param
    /// @param value
    void set(size_t row_param, size_t col_param, T value)
    {
      if (row_param > row)
      {
        throw std::out_of_range("Row index out of range: given " + std::to_string(row_param) + ", expected max index " + std::to_string(row - 1));
      }
      if (col_param > col)
      {
        throw std::out_of_range("Column index out of range: given " + std::to_string(col_param) + ", expected max index " + std::to_string(col - 1));
      }
      board[row_param][col_param] = value;
    }

    /// @brief set all elements to @value
    /// @param value
    void set_all(T value) noexcept
    {
      for (auto &&r : board)
      {
        for (auto &&elem : r)
        {
          elem = value;
        }
      }
    }

    /// @brief Get the entire board. This reference of the board is const.
    ///        It was designed for the purpose of easily reading and iterating
    ///        over the board, and const-ness grants some magnitude of safety.
    /// @return Reference to board.
    const Grid &get_board() const
    {
      return board;
    }

    // Board Metrics;

    /// @brief Count and return rows.
    /// @return Number of rows.
    size_t row_count() const
    {
      if (board.size() != row)
      {
        throw std::logic_error("Predesignated row size is different from actual row size of board. "
                               "Consider possible misoperation performed on underlying Grid.");
      }
      return row;
    }

    /// @brief Count and return columns.
    /// @return Number of columns.
    size_t col_count() const
    {
      if (board[0].size() != col)
      {
        std::cerr << board[0].size() << " " << col << std::endl;
        throw std::logic_error("Predesignated column size is different from actual column size of board. "
                               "Consider possible misoperation performed on underlying Grid.");
      }
      return col;
    }

    /// @brief Check if given coordinates are valid.
    /// @param row_param
    /// @param col_param
    /// @return true if valid, false otherwise
    bool validateCoords(size_t row_param, size_t col_param) const
    {
      return row_param < row && col_param < col and row_param >= 0 && col_param >= 0;
    }

    /// @brief Count instances of @element in current board
    /// @param element
    /// @return element count
    /// @note I don't think this will be used in this renju thing anyways.
    size_t count(const T &element) const
    {
      size_t count = 0;
      for (auto &&row : this->get_board())
        for (auto &&elem : row)
          if (elem == element)
            count++;
      return count;
    }
  };
}

#endif // BOARD_H