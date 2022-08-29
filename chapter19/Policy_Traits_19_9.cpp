//
// Created by yangfeng on 2022/8/11.
//

#include "../headers.h"

/*
19.9 策略萃取（Policy Traits）
  到目前为止，我们例子中的萃取模板被用来判断模板参数的特性：它们代表的是哪一种类型，
  作用于该类型数值的操作符的返回值的类型，以及其它特性。这一类萃取被称为特性萃取
  （property traits）。
  最为对比，某些萃取定义的是该如何处理某些类型。我们称之为策略萃取（policy traits）。
  这里会对之前介绍的策略类（policy class，我们已经指出，策略类和策略萃取之间的界限并
  不青霞）的概念进行回顾，但是策略萃取更倾向于是模板参数的某一独有特性（而策略类却
  通常和其它模板参数无关）。
  虽然特性萃取通常都可以被实现为类型函数，策略萃取却通常将策略包装进成员函数中。为
  了展示这一概念，先来看一下一个定义了特定策略（必须传递只读参数）的类型函数。
  19.9.1 只读参数类型
  在 C++和 C 中，函数的调用参数（call parameters）默认情况下是按照值传递的。这意味着，
  调用函数计算出来的参数的值，会被拷贝到由被调用函数控制的位置。大部分程序员都知道，
  对于比较大的结构体，这一拷贝的成本会非常高，因此对于这一类结构体最好能够将其按照
  常量引用（reference-to-const）或者是 C 中的常量指针（pointer-to-const）进行传递。对于
  小的结构体，到底该怎样实现目前还没有定论，从性能的角度来看，最好的机制依赖于代码
  所运行的具体架构。在大多数情况下这并没有那么关键，但是某些情况下，即使是对小的结
  构体我们也要仔细应对。
  当然，有了模板之后事情要变得更加微妙一些：我们事先并不知道用来替换模板参数的类型
  将会是多大。而且，事情也并不是仅仅依赖于结构体的大小：即使是比较小的结构体，其拷
  五车书馆
  241
  贝构造函数的成本也可能会很高，这种情况下我们应对选择按常量引用传递。
  正如之前暗示的那样，这一类问题通常应当用策略萃取模板（一个类型函数）来处理：该函
  数将预期的参数类型 T 映射到最佳的参数类型 T 或者是 T const&。作为第一步的近似，主模
  板会将大小不大于两个指针的类型按值进行传递，对于其它所有类型都按照常量引用进行传
  递：
  template<typename T>
  struct RParam {
  using Type = typename IfThenElseT<sizeof(T) <=2*sizeof(void*),
  T,
  T const&>::Type;
  };
  另一方面，对于那些另 sizeof 运算符返回一个很小的值，但是拷贝构造函数成本却很高的容
  器类型，我们可能需要分别对它们进行特化或者偏特化，就像下面这样：
  template<typename T>
  struct RParam<Array<T>> {
  using Type = Array<T> const&;
  };
  由于这一类类型在 C++中很常见，如果只将那些拥有简单拷贝以及移动构造函数的类型按值
  进行传递，当需要考虑性能因素时，再选择性的将其它一些 class 类型加入按值传递的行列
  （C++标准库中包含了 std::is_trivially_copy_constructible 和 std::is_trivially_move_constructible
  类型萃取）。
  #ifndef RPARAM_HPP
  #define RPARAM_HPP
  #include "ifthenelse.hpp"
  #include <type_traits>
  template<typename T>
  struct RParam {
  using Type = IfThenElse<(sizeof(T) <= 2*sizeof(void*)
  && std::is_trivially_copy_constructible<T>::value
  && std::is_trivially_move_constructible<T>::value),
  T,
  T const&>;
  };
  #endif //RPARAM_HPP
  无论采用哪一种方式，现在该策略都可以被集成到萃取模板的定义中，客户也可以用它们去
  实现更好的效果。比如，假设我们有两个 class，对于其中一个 class 我们指明要按值传递只
  读参数：
  #include "rparam.hpp"
  #include <iostream>
  class MyClass1 {
  五车书馆
  242
  public:
  MyClass1 () {
  }
  MyClass1 (MyClass1 const&) {
  std::cout << "MyClass1 copy constructor called\n";}
  };
  class MyClass2 {
  public:
  MyClass2 () {
  }
  MyClass2 (MyClass2 const&) {
  std::cout << "MyClass2 copy constructor called\n";
  }
  };
  // pass MyClass2 objects with RParam<> by value
  template<>
  class RParam<MyClass2> {
  public:
  using Type = MyClass2;
  };
  现在，我们就可以定义将 PParam<>用于只读参数的函数了，并对其进行调用：
  #include "rparam.hpp"
  #include "rparamcls.hpp"
  // function that allows parameter passing by value or by reference
  template<typename T1, typename T2>
  void foo (typename RParam<T1>::Type p1, typename RParam<T2>::Type p2)
  {
  …
  }
  int main()
  {
  MyClass1 mc1;
  MyClass2 mc2;
  foo<MyClass1,MyClass2>(mc1,mc2);
  }
  不幸的是，PParam 的使用有一些很大的缺点。第一，函数的声明很凌乱。第二，可能也是
  更有异议的地方，就是在调用诸如 foo()一类的函数时不能使用参数推断，因为模板参数只
  五车书馆
  243
  出现在函数参数的限制符中。因此在调用时必须显式的指明所有的模板参数。
  一个稍显笨拙的权宜之计是：使用提供了完美转发的 inline 封装函数（inline wrapper
  function），但是需要假设编译器将省略 inline 函数：
  #include "rparam.hpp"
  #include "rparamcls.hpp"
  // function that allows parameter passing by value or by reference
  template<typename T1, typename T2>
  void foo_core (typename RParam<T1>::Type p1, typename RParam<T2>::Type
  p2)
  {
  …
  }
  // wrapper to avoid explicit template parameter passing
  template<typename T1, typename T2>
  void foo (T1 && p1, T2 && p2)
  {
  foo_core<T1,T2>(std::forward<T1>(p1),std::forward<T2>(p2));
  }
  int main()
  {
  MyClass1 mc1;
  MyClass2 mc2;
  foo(mc1,mc2); // same as foo_core<MyClass1,MyClass2> (mc1,mc2)
  }
 */

using namespace std;

namespace ch19_9 {
  // 虽然特性萃取通常都可以被实现为类型函数，策略萃取却通常将策略包装进成员函数中。

  namespace case1 {
    
    template <typename T>
    struct Array { };
    
    template <bool COND, typename TrueType, typename FalseType>
    struct IfThenElseT : public true_type {
      using Type = TrueType;
    };
  
    template < typename TrueType, typename FalseType>
    struct IfThenElseT<false, TrueType, FalseType> : public false_type {
      using Type = FalseType;
    };
    
    /*
    正如之前暗示的那样，这一类问题通常应当用策略萃取模板（一个类型函数）来处理：该函
    数将预期的参数类型 T 映射到最佳的参数类型 T 或者是 T const&。作为第一步的近似，主模
    板会将大小不大于两个指针的类型按值进行传递，对于其它所有类型都按照常量引用进行传
    递：
     */
    
    template<typename T>
    struct RParam {
      using Type = typename IfThenElseT<sizeof(T) <=2*sizeof(void*),
          T,
          T const&>::Type;
    };
    
    template <bool COND, typename TrueType, typename FalseType>
    using IfThenElse = typename IfThenElseT<COND, TrueType, FalseType>::type;
  
    /*
    另一方面，对于那些另 sizeof 运算符返回一个很小的值，但是拷贝构造函数成本却很高的容
        器类型，我们可能需要分别对它们进行特化或者偏特化，就像下面这样：
    */
    template<typename T>
    struct RParam<Array<T>> {
      using Type = Array<T> const&;
    };
    
    /*
   由于这一类类型在 C++中很常见，如果只将那些拥有简单拷贝以及移动构造函数的类型按值
    进行传递，当需要考虑性能因素时，再选择性的将其它一些 class 类型加入按值传递的行列
    （C++标准库中包含了 std::is_trivially_copy_constructible 和 std::is_trivially_move_constructible
    类型萃取）。
     */
    
    template<typename T>
    struct RParam_Pro {
      using Type = IfThenElse<(   sizeof(T) <= 2*sizeof(void*)
                    && std::is_trivially_copy_constructible<T>::value
                    && std::is_trivially_move_constructible<T>::value),
                    T,
                    T const&>;
    };
    
    /*
    无论采用哪一种方式，现在该策略都可以被集成到萃取模板的定义中，客户也可以用它们去
    实现更好的效果。比如，假设我们有两个 class，对于其中一个 class 我们指明要按值传递只
    读参数：
     */
  
    class MyClass1 {
    public:
      MyClass1 () { }
      MyClass1 (MyClass1 const&) {
        std::cout << "MyClass1 copy constructor called\n";}
    };
    
    class MyClass2 {
    public:
      MyClass2 () { }
      MyClass2 (MyClass2 const&) {
        std::cout << "MyClass2 copy constructor called\n";
      }
    };
    
    // pass MyClass2 objects with RParam<> by value
    template<>
    class RParam<MyClass2> {
    public:
      using Type = MyClass2;
    };
    
    // function that allows parameter passing by value or by reference
    template<typename T1, typename T2>
    void foo (typename RParam<T1>::Type p1, typename RParam<T2>::Type p2)
    {
      // do something
    }
    
    void test()
    {
      MyClass1 mc1;
      MyClass2 mc2;
      foo<MyClass1,MyClass2>(mc1,mc2);
    
    }
  
    /*
    不幸的是，PParam 的使用有一些很大的缺点。第一，函数的声明很凌乱。第二，可能也是
    更有异议的地方，就是在调用诸如 foo()一类的函数时不能使用参数推断，因为模板参数只
    出现在函数参数的限制符中。因此在调用时必须显式的指明所有的模板参数。
    一个稍显笨拙的权宜之计是：使用提供了完美转发的 inline 封装函数（inline wrapper
    function），但是需要假设编译器将省略 inline 函数：
     */
    
    
    // function that allows parameter passing by value or by reference
    template<typename T1, typename T2>
    void foo_core (typename RParam<T1>::Type p1, typename RParam<T2>::Type p2) {
      // do something
    }
    
    // wrapper to avoid explicit template parameter passing
    template<typename T1, typename T2>
    void foo1 (T1 && p1, T2 && p2) {
      foo_core<T1,T2>(std::forward<T1>(p1),std::forward<T2>(p2));
    }
    
    void test1()
    {
      MyClass1 mc1;
      MyClass2 mc2;
      foo1(mc1,mc2); // same as foo_core<MyClass1,MyClass2> (mc1,mc2)
    }
    
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
main_Policy_Traits_19_9()
{

  return 0;
}