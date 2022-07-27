//
// Created by yangfeng on 2022/7/27.
//

/*
 *
C++一直以来都包含一些可以被用来进行编译期计算的简单方法。
    模板则进一步增加了编译期计算的可能性，而且该语言进一步的发展通常也都是在这一工具箱里进行的。
 
比较简单的情况是，可以通过它来决定是否启用某个模板，或者在多个模板之间做选择。
    不过如果有足够多的信息，编译器甚至可以计算控制流的结果。
    
事实上，C++有很多可以支持编译期编程的特性：
 
     从 C++98 开始，模板就有了编译期计算的能力，包括使用循环以及执行路径选择（然而有些人认为这是对模板特性的滥用，因为其语法不够直观）。

     基于某些限制和要求，在编译期间，可以通过部分特例化在类模板(函数模板暂不支持部分特例化)的不同实现之间做选择。

     通过 SFINAE（替换错误不算失败），可以基于不同的类型或者限制条件，在函数模板的不同实现方式之间做选择。

     在C++11和C++14中，由于可以在constexpr中使用更直观的执行路径选择方法（
        从C++14 开始，更多的语句得到支持，比如 for 循环，switch 语句等），编译期计算得到了更好的支持。
        
     C++17 则引入了编译期 if（compile-time if），通过它可以基于某些编译期的条件或限制弃用某些语句。它甚至可以用非模板函数。
*/

#include "../headers.h"

using namespace std;

/*
    模板的实例化发生在编译期间（而动态语言的泛型是在程序运行期间决定的）。
        事实证明C++模板的某些特性可以和实例化过程相结合，这样就产生了一种 C++自己内部的原始递归的“编程语言”。
         因此模板可以用来“计算一个程序的结果”。
    
    这里通过一个简单的例子来展示它们的用处。
    
    下面的代码在编译期间就能判断一个数是不是质数：
        template<unsigned p, unsigned d>            // p: number to check, d: current divisor
        struct DoIsPrime {
            static constexpr bool value = (p%d != 0) && DoIsPrime<p,d-1>::value;
        };
        
        template<unsigned p> // end recursion if divisor is 2
        struct DoIsPrime<p,2> {
            static constexpr bool value = (p%2 != 0);
        };
        
        template<unsigned p> // primary template
        struct IsPrime {
            // start recursion with divisor from p/2:
            static constexpr bool value = DoIsPrime<p,p/2>::value;
        };
        
        // special cases (to avoid endless recursion with template instantiation):
        template<>
        struct IsPrime<0> { static constexpr bool value = false; };
        
        template<>
        struct IsPrime<1> { static constexpr bool value = false; };
        
        template<>
        struct IsPrime<2> { static constexpr bool value = true; };
        
        template<>
        struct IsPrime<3> { static constexpr bool value = true; };
        
    IsPrime<>模板将结果存储在其成员 value 中。
    为了计算出模板参数是不是质数，它实例化了 DoIsPrime<>模板，这个模板会被递归展开，
        以计算 p 除以 p/2 和 2 之间的数之后是否会有余数。
*/

namespace ch8_1 {
    
    namespace case1 {
        template <unsigned p, unsigned d>
        struct DoIsPrime {
            static constexpr bool value = (p % d != 0) && DoIsPrime<p, d-1>::value;
        };
        
        template <unsigned p>
        struct DoIsPrime<p, 2> {
            static constexpr bool value = (p % 2 != 0);
        };
        
        template <unsigned p>
        struct IsPrime {
            static constexpr bool value = DoIsPrime<p, p / 2>::value;
        };
        
        template<>
        struct IsPrime<0> { static constexpr bool value = false; };
        
        template<>
        struct IsPrime<1> { static constexpr bool value = false; };
        
        template<>
        struct IsPrime<2> { static constexpr bool value = true; };
        
        template<>
        struct IsPrime<3> { static constexpr bool value = true; };
    
    }
    namespace case2 {
    
    }
    namespace case3 {
    
    }
    namespace case4 {
    
    }
    namespace case1 {
    
    }

    class Tmp {
    public:
    
    };

}

namespace test_ch8_1 {
    
    void test_case1() {
        cout << ch8_1::case1::IsPrime<17>::value << endl;
        
    }
    
}

int
//main()
main_template_meta_programming()
{
    test_ch8_1::test_case1();

    return 0;
}
