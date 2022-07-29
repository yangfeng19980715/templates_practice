//
// Created by yangfeng on 2022/7/29.
//

#ifndef TEMPLATES_PRACTICE_MYSECOND_H
#define TEMPLATES_PRACTICE_MYSECOND_H

#include <iostream>
#include <typeinfo>


template<typename T>
void printTypeOf_2(T const & );

template <typename T>
void printTypeOf_2(T const & t)
{
    std::cout << typeid(t).name() << '\n';
}

#endif //TEMPLATES_PRACTICE_MYSECOND_H
