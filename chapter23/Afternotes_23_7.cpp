//
// Created by yangfeng on 9/8/2022.
//

#include "../headers.h"

using namespace std;

/*
23.7 后记
最早的文档可查的元编程的例子是由 Erwin Unruh 实现的，并代表西门子在 C++标准委员会
上做了展示。他注意到了模板实例化过程的计算完整性，并开发了第一个元程序来证明了自
己的观点。他使用的是 Metaware 编译器，让编译器在错误信息中输出了连续的素数。在 1994
年 C++委员会上流传的代码如下（做了些修改，以使其能够在标准编译器上编译）：
// prime number computation // (modified from original from 1994 by Erwin
Unruh)
template<int p, int i>
struct is_prime {
五车书馆
317
enum { pri = (p==2) || ((p%i) && is_prime<(i>2? p:0),i-1>::pri) };
};
template<>
struct is_prime<0,0> {
enum {pri=1};
};
template<>
struct is_prime<0,1> {
enum {pri=1};
};
template<int i>
struct D {
D(void*);
};
template<int i>
struct CondNull {
static int const value = i;
};
template<>
struct CondNull<0> {
static void* value;
};
void* CondNull<0>::value = 0;
template<int i>
struct Prime_print { // primary template for loop to print prime numbers
Prime_print<i-1> a;
enum { pri = is_prime<i,i-1>::pri };
void f() {
D<i> d = CondNull<pri ? 1 : 0>::value; // 1 is an error, 0 is
ne
a.f();
}
};
template<>
struct Prime_print<1> { // full specialization to end the loop
enum {pri=0};
五车书馆
318
void f() {
D<1> d = 0;
};
};
#ifndef LAST
#define LAST 18
#endif
int main()
{
Prime_print<LAST> a;
a.f();
}
如果你试着编译以上程序，编译器会打印错误说在 Prime_print::f()中，初始化 d 时遇
到错误。错误发生在初始值时 1 的时候，因为只有一个参数为 void*的构造函数，而又只
有 0 可以被转换成 void*。下面是一个编译器报的错误的（包含在其它一些信息中）：
unruh.cpp:39:14: error: no viable conversion from ’const int’ to
’D<17>’
unruh.cpp:39:14: error: no viable conversion from ’const int’ to
’D<13>’
unruh.cpp:39:14: error: no viable conversion from ’const int’ to
’D<11>’
unruh.cpp:39:14: error: no viable conversion from ’const int’ to
’D<7>’
unruh.cpp:39:14: error: no viable conversion from ’const int’ to
’D<5>’
unruh.cpp:39:14: error: no viable conversion from ’const int’ to
’D<3>’
unruh.cpp:39:14: error: no viable conversion from ’const int’ to
’D<2>’
C++模板元编程的概念，作为一个严肃的编程工具最早是由 Todd Veldhuizen 在其文章 Using
C++ Template Metaprograms (see [VeldhuizenMeta95]) 中推广开来的（并做了一些规范化）。
Todd 在其关于 Blitz++的工作（一个 C++数值数组库，参见 Blitz++）中也对元编程（和表达式
模板技术）做了一些优化和扩展。
本书第一版和 Andrei Alexandrescu 的 Modern C++ Design 都为 C++库的爆发做出了贡献，书
中通过总结一些至今还在使用的基础技术探索了基于模板的元编程。Boost 项目则为这一爆
发带了了秩序。在早期，它引入了 MPL（元编程库，meta-programming library），这为类型
元编程（同样由 Abrahams 和 Gurtovoy 的书 “C++Template Metaprogramming” 带火）。
Template Metaprogramming” ）定义了一致的框架。
五车书馆
319
另一个重要的进步是由 Louis Dionne 贡献的，在他的 Boost.Hana 库中，使得元编程语法变得
更容易被接受。在标准委员会中，Louis 正在和 Andrew Sutton，Herb Sutter，David Vandevoorde
以及其他一些人一起，努力使元编程在语言中得到更好的支持。该工作中一个主要的部分是
什么样的程序特性应该在反射中得到支持。Matúš Chochlík, Axel Naumann 以及 David Sankel
是相关领域的主要贡献者。
在 BartonNackman 中，John J. Barton 和 Lee R. Nackman 展示了在执行计算时该如何追踪维度
成员。SIunits 库是由 Walter Brown 开发的一个用来处理物理单元的、更全面的库。而我们
在第 23.1.4 节中作为灵感来源使用的 std::chrono，则是由 Howard Hinnant 开发，专门用来
处理时间和日期的库。
 */

namespace ch23_7 {
  
  namespace case1 {
    
    // prime number computation // (modified from original from 1994 by Erwin Unruh)
    template<int p, int i>
    struct is_prime {
      enum { pri = (p == 2) || ((p % i) && is_prime<(i > 2 ? p : 0), i - 1>::pri) };
    };
    
    template<>
    struct is_prime<0, 0> {
      enum { pri = 1 };
    };
    
    template<>
    struct is_prime<0, 1> {
      enum { pri = 1 };
    };
    
    template<int i>
    struct D {
      
      D(void *);
      
    };
    
    template<int i>
    struct CondNull {
      static int const value = i;
    };
    
    template<>
    struct CondNull<0> {
      static void *value;
    };
    
    void *CondNull<0>::value = 0;
    
    template<int i>
    struct Prime_print {                 // primary template for loop to print prime numbers
      Prime_print<i - 1> a;
      enum { pri = is_prime<i, i - 1>::pri };
      
      void f() {
        // D<i> d = CondNull<pri ? 1 : 0>::value; // 1 is an error, 0 is fine
        a.f();
      }
      
    };
    
    template<>
    struct Prime_print<1> { // full specialization to end the loop
      enum { pri = 0 };
      
      void f() {
        
        // D<1> d = 0;
      };
      
    };
    
    void test() {
      
      Prime_print<18> a;
      a.f();
      
      cout << "hello, world" << endl;
    }
    
  }
  
  namespace case2 {
    
    void test() {
    
    }
    
  }
  
  namespace case3 {
    
    void test() {
    
    }
    
  }
  
  namespace case4 {
    
    void test() {
    
    }
    
  }
  
  namespace case5 {
    
    void test() {
    
    }
    
  }
  
  class Tmp {
  public:
  
  };
  
}

int
//main()
main_ch23_7()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch23_7::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch23_7::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch23_7::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch23_7::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch23_7::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;
  
  return 0;
}