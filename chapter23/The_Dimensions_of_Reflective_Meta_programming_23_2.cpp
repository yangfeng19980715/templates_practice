//
// Created by yangfeng on 2022/9/6.
//
#include "../headers.h"

using namespace std;

/*
 23.2 反射元编程的维度
上文中介绍了基于 constexpr 的“值元编程”和基于递归实例化的“类型元编程”。这两种
在现代 C++中可用的选项采用了明显不同的方式来驱动计算。事实证明“值元编程”也可以
通过模板的递归实例化来实现，在引入 C++11 的 constexpr 函数之前，这也正是其实现方式。
比如下面的代码使用递归实例化来计算一个整数的平方根：
// primary template to compute sqrt(N)
template<int N, int LO=1, int HI=N>
struct Sqrt {
// compute the midpoint, rounded up
static constexpr auto mid = (LO+HI+1)/2;
// search a not too large value in a halved interval
static constexpr auto value = (N<mid*mid) ?
Sqrt<N,LO,mid-1>::value : Sqrt<N,mid,HI>::value;
};
// partial specialization for the case when LO equals HI
template<int N, int M>
struct Sqrt<N,M,M> {
static constexpr auto value = M;
};
这个源程序使用了几乎和 23.1.1 节中的 constexpr 函数完全一样的算法，不断的二分查找包
含平方根的中间值。但是，这里元函数的输入是一个非类型模板参数，而不是一个函数参数，
用来追踪中间值边界的“局部变量”也是非类型模板参数。显然这个方法远不如 constexpr
函数友好，但是我接下来依然会探讨这段代码是如何消耗编译器资源的。
无论如何，我们已经看到元编程的计算引擎可以有多种潜在的选择。但是计算不是唯一的一
个我们应该在其中考虑相关选项的维度。一个综合的元编程解决方案应该在如下 3 个维度中
间做选择：

计算维度（Compution）

反射维度（Reflection）

生成维度（Generation）
反射维度指的是以编程的方式检测程序特性的能力。生成维度指的是为程序生成额外代码的
能力。
五车书馆
311
我们已经见过计算维度中的两个选项：递归实例化和 constexpr 计算。对于反射维度，在类
型萃取（参见第 19.6.1 节）相关章节中也介绍了其部分解决方案。虽然一些可用的类型萃取
使得某些高端的模板技术变得可能，但是这远没有包含所有的、我们所期望能够从反射机制
中获得的特性。比如给定一个类，一些应用总是倾向于在程序中访问其某些成员。目前已有
的类型萃取是基于模板实例化的，而且 C++总是会提供额外的语言特性或者是“固有的”库
元素来在编译期生成包含反射信息的类模板实例。这一方法和基于模板递归实例化进行的计
算比较相似。但是不幸的是，类模板实例会占用比较多的编译器内存，而且这部分内存要直
到编译结束才会被释放（否则的话编译时间会大大延长）。另一个被期望可以在“计算维度”
和 constexpr 运算选项组合的很好的选项是，引入一个新的标准类型来代表“反射信息”。
在第 17.9 节中对这一选项进行了讨论（目前 C++标准委员会正在对相关内容进行探讨）。
第 17.9 节中还讨论了另一种有潜力的、可以提供强大的代码生成能力的方法。在已有的 C++
语言中创建一个灵活的、通用的、用户友好的代码生成机制依然是一个被很多组织研究的、
颇有挑战的事情。但是模板实例化又总是各种代码生成机制中的一种。另外，编译器在将函
数调用扩展成小函数的 inline 方面已经足够可靠，该机制可以被用作产生代码的一种手段。
这些内容是上文中 DotProductT 的基础，并且结合强大的反射工具，现阶段已有的技术已经
可以获得优异的元编程效果了。
 */

namespace ch23_2 {
  
  namespace case1 {
    
    // primary template to compute sqrt(N)
    template<int N, int LO = 1, int HI = N>
    struct Sqrt {
      // compute the midpoint, rounded up
      static constexpr auto mid = (LO + HI + 1) / 2;
      
      // search a not too large value in a halved interval
      static constexpr auto value = (N < mid * mid) ?
                                    Sqrt<N, LO, mid - 1>::value : Sqrt<N, mid, HI>::value;
    };
    
    // partial specialization for the case when LO equals HI
    template<int N, int M>
    struct Sqrt<N, M, M> {
      static constexpr auto value = M;
    };
    
    void test() {
      cout << Sqrt<9999>::value << endl;
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
main()
//main_ch23_2()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch23_2::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch23_2::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch23_2::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch23_2::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch23_2::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;
  
  return 0;
}
