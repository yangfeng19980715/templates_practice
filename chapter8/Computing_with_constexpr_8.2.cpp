//
// Created by yangfeng on 2022/7/27.
//

#include "../headers.h"

/*
通过 constexpr 进行计算
    C++11 引入了一个叫做 constexpr 的新特性，它大大简化了各种类型的编译期计算。
    如果给定了合适的输入，constexpr 函数就可以在编译期间完成相应的计算。
    虽然 C++11 对 constexpr函数的使用有诸多限制（比如 constexpt 函数的定义通常都只能包含一个 return 语句），
        但是在 C++14 中这些限制中的大部分都被移除了。
        
    当然，为了能够成功地进行 constexpr 函数中的计算，
        依然要求各个计算步骤都能在编译期进行：目前堆内存分配和异常抛出都不被支持。
        
    在 C++11 中，判断一个数是不是质数的实现方式如下：
        constexpr bool
        doIsPrime (unsigned p, unsigned d)                      // p: number to check, d: current divisor
        {
            return d!=2 ? (p%d!=0) && doIsPrime(p,d-1)          // check this and smaller divisors
                                    : (p%2!=0);                 // end recursion if divisor is 2
        }
        constexpr bool isPrime (unsigned p)
        {
            return p < 4 ? !(p<2)                               // handle special cases
                            : doIsPrime(p,p/2);                 // start recursion with divisor from p/2
        }
        
    为了满足 C++11 中只能有一条语句的要求，此处只能使用条件运算符来进行条件选择。
    不过由于这个函数只用到了 C++的常规语法，因此它比第一版中，依赖于模板实例化的代码要容易理解的多。
    
    在 C++14 中，constexpr 函数可以使用常规 C++代码中大部分的控制结构。
    因此为了判断一个数是不是质数，可以不再使用笨拙的模板方式（C++11 之前）以及略显神秘的单行代码方式（C++11），
        而直接使用一个简单的 for 循环：
        
        constexpr bool isPrime (unsigned int p)
        {
            for (unsigned int d=2; d<=p/2; ++d) {
                if (p % d == 0) {
                    return false;                               // found divisor without remainder
                }
            }
            return p > 1;                                       // no divisor without remainder found
        }
        
    在 C++11 和 C++14 中实现的 constexpr isPrime()，都可以通过直接调用：
        isPrime(9) 来判断 9 是不是一个质数。
        
    但是上面所说的“可以”在编译期执行，并不是一定会在编译期执行。
     在需要编译期数值的上下文中（比如数组的长度和非类型模板参数），
      编译器会尝试在编译期对被调用的 constexpr 函数进行计算，此时如果无法在编译期进行计算，
       就会报错（因为此处必须要产生一个常量）。
      
    在其他上下文中，编译期可能会也可能不会尝试进行编译期计算，如果在编译期尝试了，
        但是现有条件不满足编译期计算的要求，那么也不会报错，相应的函数调用被推迟到运行期间执行。
        
    比如：
    
        constexpr bool b1 = isPrime(9);                     // evaluated at compile time
    会在编译期进行计算（因为 b1 被 constexpr 修饰）。
    
    而对
        const bool b2 = isPrime(9);                         // evaluated at compile time if in namespace scope
     如果 b2 被定义于全局作用域或者 namespace 作用域，也会在编译期进行计算。
     如果 b2 被定义于块作用域（{}内），那么将由编译器决定是否在编译期间进行计算。
     
    下面这个例子就属于这种情况：
        bool fiftySevenIsPrime() {
            return isPrime(57);                             // evaluated at compile or running time
        }
    此时是否进行编译期计算将由编译期决定。
    
    另一方面，在如下调用中：
        int x;
        std::cout << isPrime(x);                            // evaluated at run time
    不管 x 是不是质数，调用都只会在运行期间执行。
*/

using namespace std;

namespace ch8_2 {
    
    
    // 为了满足 C++11 中只能有一条语句的要求，此处只能使用条件运算符来进行条件选择。
    namespace cpp11 {
        
        constexpr bool
        doIsPrime(unsigned p, unsigned d) {
            return d != 2 ? (p % d != 0) && doIsPrime(p, d - 1)  // check this and smaller divisors
                    : (p % 2 != 0);                          // end recursion if divisor is 2
        }
        
        constexpr bool
        isPrime(unsigned p) {
            return p < 4 ? !(p < 2)                          // handle special cases
                   : doIsPrime(p, p / 2);                    // start recursion with divisor from p/2
        }
        
    }
    
    /*
    在 C++14 中，constexpr 函数可以使用常规 C++代码中大部分的控制结构。
     因此为了判断一个数是不是质数，可以不再使用笨拙的模板方式（C++11 之前）以及
        略显神秘的单行代码方式（C++11），而直接使用一个简单的 for 循环。
    */
    namespace cpp14 {
        constexpr bool
        isPrime(unsigned p) {
            for (unsigned d = 2; d <= p / 2; ++d)
                if (p % d == 0)
                    return false;
            
            return p > 1;
        }
    
    }

    class Tmp {
    public:
    
    };

}

namespace test_ch8_2 {
    void test_cpp11() {
        cout << "17 is prime? " << (ch8_2::cpp11::isPrime(17) ? "yes" : "no") << endl;
        cout << "10 is prime? " << (ch8_2::cpp11::isPrime(10) ? "yes" : "no") << endl;
    }
    
    void test_cpp14() {
        cout << "17 is prime? " << (ch8_2::cpp14::isPrime(17) ? "yes" : "no") << endl;
        cout << "10 is prime? " << (ch8_2::cpp14::isPrime(10) ? "yes" : "no") << endl;
    }
    
}

int
//main()
main_computing_with_constexpr()
{
    test_ch8_2::test_cpp11();
    test_ch8_2::test_cpp14();

    return 0;
}
