/* Header that includes basic underlying types for the go board.
 * Contents of this file include:
 * namespace GosFrontline: namespace enwrapping all relevant interfaces
 * TODO: add in classes and functions as they are written.
 */
#ifndef BOARD_H
#define BOARD_H
#include <cstring>
#include <stdexcept>
//#include "GFHelp"

//NOTE: neither g++ nor clang supports this.
//#if __cplusplus >= 202002L
//  #include <format>
//#endif //C++ version

namespace GosFrontline{
  template<typename T>
  class PackedArray{
    private:
      T* arr;
      int array_length;
    public:
      PackedArray();
      PackedArray(std::size_t);
      PackedArray(std::size_t, T);
      ~PackedArray();
      std::size_t len();
      T& operator[](std::size_t);
   };

/*
  template<typename T>
  class Vector2D{
    private:
      PackedArray<PackedArray<T>> vec;
      int length, width;
    public:
      basicBoard();
      basicBoard(std::size_t length,int width = -1);
      void fill_with(T filler);
  };*/
}

#endif //BOARD_H
