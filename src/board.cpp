#include "board.h"

namespace GosFrontline{

    template<typename T>
    PackedArray<T>::PackedArray(){
      array_length = 0;
      arr = new T[array_length];
    }

    template<typename T>
    PackedArray<T>::PackedArray(std::size_t length){
      array_length = length;
      arr = new T[array_length];
    }

    template<typename T>
    PackedArray<T>::PackedArray(std::size_t length, T initializer){
        array_length = length;
        arr = new T[array_length];
        std::memset(arr, initializer, sizeof(T) * array_length);
    }

    template<typename T>
    PackedArray<T>::~PackedArray(){
        delete[] arr;
        array_length = 0;
    }

    template<typename T>
    std::size_t PackedArray<T>::len(){
        return array_length;
    }

    template<typename T>
    T& PackedArray<T>::operator[](std::size_t index){
        if (index >= len()) throw std::out_of_range(
//#if __cplusplus >= 202002L
//            std::format
//#else
//            GFHelp::format
//#endif //C++ version verifier
            "Index out of range in PackedArray: got {}, max index is {} (length is {})."
//            index, len() - 1, len()
            ); //end of range verifier
        return arr[index];
    }

}//GosFrontline

template class GosFrontline::PackedArray<int>;
