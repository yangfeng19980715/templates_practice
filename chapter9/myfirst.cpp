//
// Created by yangfeng on 2022/7/29.
//

#include "myfirst.h"
#include <iostream>
#include <typeinfo>

template <typename T>
void printTypeOf(T const & t)
{
    std::cout << typeid(t).name() << '\n';
}
