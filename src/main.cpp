#include <iostream>
#include "board.h"

int main(){
  GosFrontline::PackedArray<int> trying(static_cast<std::size_t>(10), 0);
  for (int i = 0; i < trying.len(); i++){
      std::cout << trying[i] << " ";
  }
  return 0;
}
