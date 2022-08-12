//
// Created by yangfeng on 2022/8/11.
//

#include "../headers.h"

/*
第 20 章
基 于 类 型 属 性 的 重 载
（Overloading on Type Properties）
函数重载使得相同的函数名能够被多个函数使用，只要能够通过这些函数的参数类型区分它
们就行。比如：
void f (int);
void f (char const*);
对于函数模板，可以在类型模式上进行重载，比如针对指向 T 的指针或者 Array<T>：
template<typename T> void f(T*);
template<typename T> void f(Array<T>);
在类型萃取（参考第 19 章）的概念流行起来之后，很自然地会想到基于模板参数对函数模
板进行重载。比如：
template<typename Number> void f(Number); // only for numbers
template<typename Container> void f(Container);// only for containers
但是，目前 C++还没有提供任何可以直接基于类型属性进行重载的方法。事实上，上面的两
个模板声明的是完全相同的函数模板，而不是进行了重载，因为在比较两个函数模板的时候
不会比较模板参数的名字。
幸运的是，有比较多的基于类型特性的技术，可以被用来实现类似于函数模板重载的功能。
本章将会讨论这些相关技术，以及为什么要实现这一类重载的原因。
 */

using namespace std;

namespace ch20_1 {
    
    /*
20.1 算法特化（我更愿意称之为算法重载，见注释）
    函数模板重载的一个动机是，基于算法适用的类型信息，为算法提供更为特化的版本。考虑
    一个交换两个数值的 swap()操作：
    template<typename T>
    void swap(T& x, T& y)
    {
    T tmp(x);
    x = y;
    y = tmp;
    }
    这一实现用到了三次拷贝操作。但是对于某些类型，可以有一种更为高效的 swap()实现，比
    如对于存储了指向具体数组内容的指针和数组长度的 Array<T>:
    template<typename T>
    五车书馆
    246
    void swap(Array<T>& x, Array<T>& y)
    {
    swap(x.ptr, y.ptr);
    swap(x.len, y.len);
    }
    俩种 swap()实现都可以正确的交换两个 Array<T>对象的内容。但是，后一种实现方式的效率
    要高很多，因为它利用了 Array<T>中额外的（具体而言，是 ptr 和 len 以及它们各自的职责）、
    不为其它类型所有的特性。因此后一种实现方式要（在概念上）比第一种实现方式更为“特
    化”，这是因为它只为适用于前一种实现的类型的一个子集提供了交换操作。幸运的是，基
    于函数模板的部分排序规则（partial ordering rules，参见 16.2.2 节），第二种函数模板也是
    更为特化的，在有更为特化的版本（也更高效）可用的时候，编译器会优先选择该版本，在
    其不适用的时候，会退回到更为泛化的版本（可能会不那么高效）。
    在一个泛型算法中引入更为特化的变体，这一设计和优化方式被称为算法特化（algorithm
    specialization）。更为特化的变体适用于泛型算法诸多输入中的一个子集，这个子集可以通
    过特定的类型或者是类型的属性来区分，针对这个子集，该特化版本通常要比泛型算法的一
    般版本高效的多。
    在适用的情况下更为特化的算法变体会自动的被选择，这一点对算法特化的实现至关重要，
    调用者甚至都不需要知道具体变体的存在。在我们的 swap()例子中，具体实现方式是用（在
    概念上）更为特化的函数模板去重载最泛化的模板，同时确保了在 C++的部分排序规则
    （partial ordering rules）中更为特化的函数模板也是更为特化的。
    并不是所有的概念上更为特化的算法变体，都可以被直接转换成提供了正确的部分排序行为
    （partial ordering behavior）的函数模板。比如我们下面的这个例子。
    函数模板 advanceIter()（类似于 C++标准库中的 std::advance()）会将迭代器 x 向前迭代 n 步。
    这一算法可以用于输入的任意类型的迭代器：
    template<typename InputIterator, typename Distance>
    void advanceIter(InputIterator& x, Distance n)
    {
    while (n > 0) { //linear time
    ++x;
    --n;
    }
    }
    对于特定类型的迭代器（比如提供了随机访问操作的迭代器)，我们可以为该操作提供一个
    更为高效的实现方式：
    template<typename RandomAccessIterator, typename Distance>
    void advanceIter(RandomAccessIterator& x, Distance n) {
    x += n; // constant time
    }
    五车书馆
    247
    但是不幸的是，同时定义以上两种函数模板会导致编译错误，正如我们在序言中介绍的那样，
    这是因为只有模板参数名字不同的函数模板是不可以被重载的。本章剩余的内容会讨论能够
    允许我们实现类似上述函数模板重载的一些技术。
     */

    namespace case1 {
        
        template <typename T>
        struct Array {
            T * ptr;
            size_t len;
        };
    
        template<typename T>
        void swap(T& x, T& y) {
            T tmp(x);
            x = y;
            y = tmp;
        }
    
        template<typename T>
        void swap(Array<T>& x, Array<T>& y) {
            swap(x.ptr, y.ptr);
            swap(x.len, y.len);
        }
    
        template<typename InputIterator, typename Distance>
        void advanceIter(InputIterator& x, Distance n)
        {
            while (n > 0) { //linear time
                ++x;
                --n;
            }
        }
        
        /*   ERROR , not overload
        template<typename RandomAccessIterator, typename Distance>
        void advanceIter(RandomAccessIterator& x, Distance n) {
            x += n; // constant time
        }
         */
        
    }
    
    namespace case2 {
    }
    
    namespace case3 {
    }
    
    namespace case4 {
    }
    
    namespace case5 {
    
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_Algorithm_Specialization_20_1()
{

    return 0;
}