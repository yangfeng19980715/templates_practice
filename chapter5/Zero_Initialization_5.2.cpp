//
// Created by yangfeng on 2022/7/20.
//
#include "../headers.h"

using namespace std;

/*
  对于内置类型，最好显式的调用其默认构造函数来将它们初始化成 0
   （对于bool 类型，初始化为 false，对于指针类型，初始化成 nullptr）。
   通过下面你的写法就可以保证即使是内置类型也可以得到适当的初始化：
        template<typename T>
        void foo()
        {
            T x{}; // x is zero (or false) if T is a built-in type
        }
    这种初始化的方法被称为“值初始化（value initialization）”，
    它要么调用一个对象已有的构造函数，要么就用零来初始化这个对象。
    即使它有显式的构造函数也是这样。
    
    对于用花括号初始化的情况，如果没有可用的默认构造函数，
    它还可以使用列表初始化构造函数（initializer-list constructor）。
 */

namespace ch5_2 {

    class Tmp {
    public:
    
    };

}

int
//main()
main_zero_initialization()
{

    return 0;
}
