//
// Created by yangfeng on 9/7/2022.
//

#include "../headers.h"

using namespace std;

/*
 23.6 枚举值还是静态常量
在早期 C++中，枚举值是唯一可以用来在类的声明中、创建可用于类成员的“真正的常量”
（也称常量表达式）的方式。比如通过它们可以定义 Pow3 元程序来计算 3 的指数：
// primary template to compute 3 to the Nth
template<int N>
struct Pow3 {
enum { value = 3 * Pow3<N-1>::value };
};
// full specialization to end the recursion
template<>
struct Pow3<0> {
enum { value = 1 };
};
五车书馆
316
在 C++98 标准中引入了类内静态常量初始化的概念，因此 Pow3 元程序可以被写成这样：
// primary template to compute 3 to the Nth
template<int N
struct Pow3 {
static int const value = 3 * Pow3<N-1>::value;
};
// full specialization to end the recursion
template<>
struct Pow3<0> {
static int const value = 1;
};
但是上面代码中有一个问题：静态常量成员是左值（参见附录 B）。因此如果我们有如下函
数：
void foo(int const&);
然后我们将元程序的结果传递给它：
foo(Pow3<7>::value);
编译器需要传递 Pow3<7>::value 的地址，因此必须实例化静态成员并为之开辟内存。这
样该计算就不是一个纯正的“编译期”程序了。
枚举值不是左值（也就是说它们没有地址）。因此当将其按引用传递时，不会用到静态内存。
几乎等效于将被计算值按照字面值传递。因此本书第一版建议在这一类应用中使用枚举值，
而不是静态常量。
不过在 C++中，引入了 constexpr 静态数据成员，并且其使用不限于整型类型。这并没有解
决上文中关于地址的问题，但是即使如此，它也是用来产生元程序结果的常规方法。其优点
是，它可以有正确的类型（相对于人工的枚举类型而言），而且当用 auto 声明静态成员的
类型时，可以对其类型进行推断。C++17 则引入了 inline 的静态数据成员，这解决了上面提
到的地址问题，而且可以和 constexpr 一起使用。
 */

namespace ch23_6 {

  namespace case1 {
    
    // primary template to compute 3 to the Nth
    template<int N>
    struct Pow3 {
      enum { value = 3 * Pow3<N-1>::value };
    };
    
    // full specialization to end the recursion
    template<>
    struct Pow3<0> {
      enum { value = 1 };
    };
    
    
  
    void test() {
      
      constexpr int ret = Pow3<10>::value;
      
      cout << ret << endl;
      
      cout << "hello, world" << endl;
    }
    
  }
  
  namespace case2 {
  
    // 在 C++98 标准中引入了类内静态常量初始化的概念，因此 Pow3 元程序可以被写成这样：
    // primary template to compute 3 to the Nth
    template<int N>
    struct Pow3 {
      static int const value = 3 * Pow3<N-1>::value;
    };
    
    // full specialization to end the recursion
    template<>
    struct Pow3<0> {
      static int const value = 1;
    };
  
    /*
    但是上面代码中有一个问题：静态常量成员是左值（参见附录 B）。因此如果我们有如下函数：
      void foo(int const&);
      
    然后我们将元程序的结果传递给它：
      foo(Pow3<7>::value);
      
    编译器需要传递 Pow3<7>::value 的地址，因此必须实例化静态成员并为之开辟内存。
    这样该计算就不是一个纯正的“编译期”程序了。
    
    枚举值不是左值（也就是说它们没有地址）。因此当将其按引用传递时，不会用到静态内存。
    几乎等效于将被计算值按照字面值传递。因此本书第一版建议在这一类应用中使用枚举值，而不是静态常量。
    
    不过在 C++中，引入了 constexpr 静态数据成员，并且其使用不限于整型类型。
     这并没有解决上文中关于地址的问题，但是即使如此，它也是用来产生元程序结果的常规方法。
     
    其优点是，它可以有正确的类型（相对于人工的枚举类型而言），
       而且当用 auto 声明静态成员的类型时，可以对其类型进行推断。
       C++17 则引入了 inline 的静态数据成员，这解决了上面提到的地址问题，而且可以和 constexpr 一起使用。
      */
  
    void test() {
    
    }
    
  }
  
  namespace case3 {
    
    template<int N>
    struct Pow3 {
      static inline int constexpr value = 3 * Pow3<N-1>::value;
    };
  
    // full specialization to end the recursion
    template<>
    struct Pow3<0> {
      static int const value = 1;
    };
  
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
main_ch23_6()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch23_6::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch23_6::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch23_6::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch23_6::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch23_6::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}