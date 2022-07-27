//
// Created by yangfeng on 2022/7/27.
//

/*
通过部分特例化进行路径选择
    诸如 isPrime() 这种在编译期进行相关测试的功能，有一个有意思的应用场景：
        可以在编译期间通过部分特例化在不同的实现方案之间做选择。
        
    比如，可以以一个非类型模板参数是不是质数为条件，在不同的模板之间做选择：
        // primary helper template:
        template<int SZ, bool = isPrime(SZ)>
        struct Helper;
        
        // implementation if SZ is not a prime number:
        template<int SZ>
        struct Helper<SZ, false>
        {
            …
        };
        
        // implementation if SZ is a prime number:
        template<int SZ>
        struct Helper<SZ, true>
        {
            …
        };
        
        template<typename T, std::size_t SZ>
        long foo (std::array<T,SZ> const& coll)
        {
            Helper<SZ> h; // implementation depends on whether array has prime number as size
            …
        }
        
    这里根据参数 std::array<>的 size 是不是一个质数，实现了两种 Helper<>模板。
      这一偏特例化的使用方法，被广泛用于基于模板参数属性，在不同模板实现方案之间做选择。
    
    在上面的例子中，对两种可能的情况实现了两种偏特例化版本。
      但是也可以将主模板用于其中一种情况，然后再特例化一个版本代表另一种情况：
        // primary helper template (used if no specialization fits):
        template<int SZ, bool = isPrime(SZ)>
        struct Helper
        {
        };
        
        // special implementation if SZ is a prime number:
        template<int SZ>
        struct Helper<SZ, true>
        {
        };
        
    由于函数模板不支持部分特例化，当基于一些限制在不同的函数实现之间做选择时，
        必须要使用其它一些方法：
             使用有 static 函数的类，
             使用 6.3 节中介绍的 std::enable_if，
             使用下一节将要介绍的 SFINAE 特性，
             或者使用从 C++17 开始生效的编译期的 if 特性，这部分内容会在 8.5 节进行介绍。
            
            第 20 章介绍了基于限制条件，在不同的函数实现之间做选择的相关技术。
*/

#include "../headers.h"

using namespace std;

namespace nmsp {
    
    constexpr bool
    isPrime(unsigned p) {
        for (unsigned d = 2; d <= p / 2; ++d)
            if (p % d == 0)
                return false;
        
        return p > 1;
    }
    
    namespace case1 {
        template <int SZ, bool = isPrime(SZ)>
        struct Helper;
        
        template <int SZ>
        struct Helper<SZ, false> { };
    
        template <int SZ>
        struct Helper<SZ, true> { };
        
        template <typename T, std::size_t SZ>
        long foo(std::array<T, SZ> const& coll) {
            Helper<SZ> h;
        }
    
    }
    
    namespace case2 {
        // primary helper template (used if no specialization fits):
        template<int SZ, bool = isPrime(SZ)>
        struct Helper { };
        
        // special implementation if SZ is a prime number:
        template<int SZ>
        struct Helper<SZ, true> { };
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_execution_path_selection_with_partial_specialization()
{

    return 0;
}
