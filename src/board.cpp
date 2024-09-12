#include "board.h"

template<typename T>
GosFrontline::PackedArray::PackedArray(){
  array_length = 0;
  arr = new T[array_length];
}

template<typename T>
GosFrontline::PackedArray::PackedArray(int length){
  array_length = length;
  arr = new T[array_length];
}




