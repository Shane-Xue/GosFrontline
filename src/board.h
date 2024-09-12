/* Header that includes basic underlying types for the go board.
 * Contents of this file include:
 * namespace GosFrontline: namespace enwrapping all relevant interfaces
 * TODO: add in classes and functions as they are written.
 */
#ifndef BOARD
#define BOARD

namespace GosFrontline{
  template<typename T>
  class PackedArray{
    private:
      T* arr;
      int array_length;
    public:
      PackedArray();
      PackedArray(int);
      PackedArray(int, T);
      ~PackedArray();
      int len();
      T& operator[](std::size_t)
   };

/*
  template<typename T>
  class Vector2D{
    private:
      PackedArray<PackedArray<T>> vec;
      int length, width;
    public:
      basicBoard();
      basicBoard(int length,int width = -1);
      void fill_with(T filler);
  };*/
}

#endif //board.h
