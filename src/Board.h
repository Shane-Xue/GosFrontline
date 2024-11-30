#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <utility>
#include <type_traits>


namespace GosFrontline{
  template<typename T>
  class Board{
  private:
    using Grid = std::vector<std::vector<T>>;
    Grid board;
    int col, row;
  public:
    //Initializers
    Board(){};
    Board(int n, T init_val = T()) : 
      board(std::vector<std::vector<T>>(n, std::vector<T>(n, init_val))),
      col(n), row(n){};

    //Copy and Move initializers and operator=
    Board(const Board& other) :
      board(other.board), col(other.col), row(other.row){};
    Board operator=(const Board& other){
      board = other.board;
      col = other.col;
      row = other.row;
      return *this;
    }

    Board(Board&& other) noexcept :
      col(other.col), row(other.row){
        board = std::move(other.board);
        other.col = 0;
        other.row = 0;
    }
    Board operator=(Board&& other) noexcept{
      board = std::move(other.board);
      other.col = 0;
      other.row = 0;
      return *this;
    }

    //Accessing elements

    /// @brief take row, but disallow modification of vector
    /// @param index 
    /// @return const vector<T>&
    const vector<T>& operator[](size_t index){
      return board[index];
    }

    /// @brief access specific element that enables modifying
    /// @tparam IndexType 
    /// @param indices 
    /// @return non-const T&
    template<typename IndexType>
    T& operator [](pair<IndexType, IndexType> indices){
      return board[indices.first][indices.second];
    }

    /// @brief const version of pair operator[]
    /// @tparam IndexType 
    /// @param indices 
    /// @return const T& value
    template<typename IndexType>
    const T& operator[](pair<IndexType, IndexType> indices){
      return board[indices.first][indices.second];
    }

    // Functions for modification of board
    // Checks against index issues

    T& at(size_t row_param, size_t col_param){
      return board.at(row_param).at(col_param);
    }

    void set(size_t row_param, size_t col_param, T value){
      if (row_param > row)
      {
        throw std::out_of_range("Row index out of range: given " + std::to_string(row_param)
          +", expected max index " + std::to_string(row - 1));
      }
      if (col_param > col)
      {
        throw std::out_of_range("Column index out of range: given " + std::to_string(col_param)
          +", expected max index " + std::to_string(col - 1));
      }
      board[row_param][col_param] = value;
    }

    void set_all(T value) nothrow{
      for (auto &&r : board)
      {
        for (auto &&elem : r)
        {
          elem = value;
        }
      }
    }

    
    
  };
}

#endif //BOARD_H