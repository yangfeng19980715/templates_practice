//
// Created by yangfeng on 9/7/2022.
//

#include "../headers.h"

using namespace std;

/*
 23.3 递归实例化的代价
现在来分析第 23.2 节中介绍的 Sqrt<>模板。主模板是由模板参数 N（被计算平方根的值）
和其它两个可选参数触发的、常规的递归计算。两个可选的参数分别是结果的上限和下限。
如果只用一个参数调用该模板，那么其平方根最小是 1，最大是其自身。
递归会按照二分查找的方式进行下去。在模板内部会计算 value 是在从 LO 到 HI 这个区间的
上半部还是下半部。这一分支判断是通过运算符?:实现的。如果 mid2 比 N 大，那么就继续
在上半部分查找，否在就在下半部分查找。
偏特例化被用来在 LO 和 HI 的值都为 M 的时候结束递归，这个值也就是我们最终所要计算
的结果。
实例化模板的成本并不低廉：即使是比较适中的类模板，其实例依然有可能占用数 KB 的内
存，而且这部分被占用的内存在编译完成之前不可以被回收利用。我们先来分析一个使用了
Sqrt 模板的简单程序：
#include <iostream>
#include "sqrt1.hpp"
int main()
{
std::cout << "Sqrt<16>::value = " << Sqrt<16>::value << ’\n’;
std::cout << "Sqrt<25>::value = " << Sqrt<25>::value << ’\n’;
std::cout << "Sqrt<42>::value = " << Sqrt<42>::value << ’\n’;
std::cout << "Sqrt<1>::value = " << Sqrt<1>::value << ’\n’;
}
五车书馆
312
表达式
Sqrt<16>::value
被扩展成
Sqrt<16,1,16>::value
在模板内部，元程序按照如下方式计算 Sqrt<16,1,16>::value
的值：
mid = (1+16+1)/2
= 9
value = (16<9*9) ? Sqrt<16,1,8>::value
: Sqrt<16,9,16>::value
= (16<81) ? Sqrt<16,1,8>::value
: Sqrt<16,9,16>::value
= Sqrt<16,1,8>::value
接着这个值会被以 Sqrt<16,1,8>::value 的形式计算，其会被接着展开为：
mid = (1+8+1)/2
= 5
value = (16<5*5) ? Sqrt<16,1,4>::value
: Sqrt<16,5,8>::value
= (16<25) ? Sqrt<16,1,4>::value
: Sqrt<16,5,8>::value
= Sqrt<16,1,4>::value
类似的，Sqrt<16,1,4>::value 被分解为如下形式：
mid = (1+4+1)/2
= 3
value = (16<3*3) ? Sqrt<16,1,2>::value
: Sqrt<16,3,4>::value
= (16<9) ? Sqrt<16,1,2>::value
: Sqrt<16,3,4>::value
= Sqrt<16,3,4>::value
最终，Sqrt<16,3,4>::value 产生出如下结果：
mid = (3+4+1)/2
= 4
value = (16<4*4) ? Sqrt<16,3,3>::value
: Sqrt<16,4,4>::value
= (16<16) ? Sqrt<16,3,3>::value
: Sqrt<16,4,4>::value
= Sqrt<16,4,4>::value
五车书馆
313
然后这一递归过程会被 Sqrt<16,4,4>::value 终结，因为对它的调用会匹配到模板的特
化版本上（上限和下限相同）。因此最终的结果是：
value = 4
23.3.1 追踪所有的实例化过程
上文中主要分析了被用来计算 16 的平方根的实例化过程。但是当编译期计算：
(16<=8*8) ? Sqrt<16,1,8>::value
: Sqrt<16,9,16>::value
的 时 候 ， 它 并 不 是 只 计 算 真 正 用 到 了 的 分 支 ， 同 样 也 会 计 算 没 有 用 到 的 分 支
（Sqrt<16,9,16>）。而且，由于代码试图通过运算符::访问最终实例化出来的类的成员，
该类中所有的成员都会被实例化。也就是说 Sqrt<16,9,16>的完全实例化会导致
Sqrt<16,9,12>和 Sqrt<16,13,16>都会被完全实例化。仔细分析以上过程，会发现最终
会实例化出很多的实例，数量上几乎是 N 的两倍。
幸运的是，有一些技术可以被用来降低实例化的数目。为了展示其中一个重要的技术，我们
按照如下方式重写了 Sqrt 元程序：
#include "ifthenelse.hpp"
// primary template for main recursive step
template<int N, int LO=1, int HI=N>
struct Sqrt {
// compute the midpoint, rounded up
static constexpr auto mid = (LO+HI+1)/2;
// search a not too large value in a halved interval
using SubT = IfThenElse<(N<mid*mid),
Sqrt<N,LO,mid-1>,
Sqrt<N,mid,HI>>;
static constexpr auto value = SubT::value;
};
// partial specialization for end of recursion criterion
template<int N, int S>
struct Sqrt<N, S, S> {
static constexpr auto value = S;
};
代码中主要的变化是使用了 IfThenElse 模板，在第 19.7.1 节有对它的介绍。回忆一下，
IfThenElse 模板被用来基于一个布尔常量在两个类型之间做选择。如果布尔型常量是 true，
那么会选择第一个类型，否则就选择第二个类型。一个比较重要的、需要记住的点是：为一
个类模板的实例定义类型别名，不会导致 C++编译器去实例化该实例。因此使用如下代码时：
using SubT = IfThenElse<(N<mid*mid),
Sqrt<N,LO,mid-1>,
五车书馆
314
Sqrt<N,mid,HI>>;
既不会完全实例化 Sqrt<N,LO,mid-1>也不会完全实例化 Sqrt<N,mid,HI>。
在调用 SubT::value 的时候，只有真正被赋值给 SubT 的那一个实例才会被完全实例化。
和之前的方法相比，这会让实例化的数量和 log2N 成正比：当 N 比较大的时候，这会大大降
低元程序实例化的成本。
 */

namespace ch23_3 {
  
  namespace case1 {
    
    std::conditional<false, int, float>::type func() {
      cout << __func__ << endl;
      return 1.0f;
    }
    
    template<bool cond, typename T, typename U>
    struct IfThenElseT {
      using Type = T;
    };
    
    template<typename T, typename U>
    struct IfThenElseT<false, T, U> {
      using Type = U;
    };
    
    template<bool cond, typename T, typename U>
    using IfThenElse = typename IfThenElseT<cond, T, U>::Type;
    
    // primary template for main recursive step
    template<int N, int LO = 1, int HI = N>
    struct Sqrt {
      // compute the midpoint, rounded up
      static constexpr auto mid = (LO + HI + 1) / 2;
      
      //  一个比较重要的、需要记住的点是：为一个类模板的实例定义类型别名，不会导致 C++编译器去实例化该实例。
      //    在调用 SubT::value 的时候，只有真正被赋值给 SubT 的那一个实例才会被完全实例化。
      // search a not too large value in a halved interval
      using SubT = IfThenElse<(N < mid * mid), Sqrt<N, LO, mid - 1>, Sqrt<N, mid, HI>>;
      
      static constexpr auto value = SubT::value;
    };
    
    // partial specialization for end of recursion criterion
    template<int N, int S>
    struct Sqrt<N, S, S> {
      static constexpr auto value = S;
    };
    
    void test() {
      
      cout << Sqrt<9999>::value << endl;
      
      func();
      
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
main_ch23_3()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch23_3::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch23_3::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch23_3::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch23_3::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch23_3::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;
  
  return 0;
}