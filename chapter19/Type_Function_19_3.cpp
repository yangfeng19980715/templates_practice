//
// Created by yangfeng on 2022/8/2.
//

#include "../headers.h"
/*
类型函数（Type Function）
最初的示例说明我们可以基于类型定义行为。传统上我们在 C 和 C++里定义的函数可以被更
明确的称为值函数（value functions）：它们接收一些值作为参数并返回一个值作为结果。
对于模板，我们还可以定义类型函数（type functions）：它们接收一些类型作为参数并返回
一个类型或者常量作为结果。
一个很有用的内置类型函数是 sizeof，它返回了一个代表了给定类型大小（单位是 byte）的
常数。类模板依然可以被用作类型函数。此时类型函数的参数是模板参数，其结果被提取为
成员类型或者成员常量。比如，sizeof 运算符可以被作为如下接口提供：
  #include <cstddef>
  #include <iostream>
  
  template<typename T>
  struct TypeSize {
    static std::size_t const value = sizeof(T);
  };
  
  int main()
  {
    std::cout << "TypeSize<int>::value = " << TypeSize<int>::value << endl;
  }
这看上去可能没有那么有用，因为我们已经有了一个内置的 sizeof 运算符，但是请注意此处
的 TypeSize<T>是一个类型，它可以被作为类模板参数传递。或者说，TypeSize 是一个模板，
也可以被作为模板模板参数传递。
在接下来的内容中，我们设计了一些更为通用的类型函数，可以按照上述方式将它们用作萃
取类。
*/

using namespace std;

namespace ch19_3 {
  
  /*
19.3.1 元素类型（Element Type）
  假设我们有很多的容器模板，比如 std::vector<>和 std::list<>，也可以包含内置数组。我们希
  望得到这样一个类型函数，当给的一个容器类型时，它可以返回相应的元素类型。这可以通
  过偏特化实现：
  #include <vector>
  #include <list>
  template<typename T>
  struct ElementT; // primary template
  template<typename T>
  struct ElementT<std::vector<T>> { //partial specialization for
  std::vector
  using Type = T;
  };
  template<typename T>
  struct ElementT<std::list<T>> { //partial specialization for std::list
  using Type = T;
  };
  …
  template<typename T, std::size_t N>
  struct ElementT<T[N]> { //partial specialization for arrays of known
  五车书馆
  189
  bounds
  using Type = T;
  };
  template<typename T>
  struct ElementT<T[]> { //partial specialization for arrays of unknown
  bounds
  using Type = T;
  };
  …
  注意此处我们应该为所有可能的数组类型提供偏特化（详见第 5.4 节）。
  我们可以想下面这样使用这些类型函数：
  #include "elementtype.hpp"
  #include <vector>
  #include <iostream>
  #include <typeinfo>
  template<typename T>
  void printElementType (T const& c)
  {
  std::cout << "Container of " <<
  typeid(typename ElementT<T>::Type).name() << " elements.\n";
  }
  int main()
  {
  std::vector<bool> s;
  printElementType(s);
  int arr[42];
  printElementType(arr);
  }
  偏特化的使用使得我们可以在容器类型不知道具体类型函数存在的情况下去实现类型函数。
  但是在某些情况下，类型函数是和其所适用的类型一起被设计的，此时相关实现就可以被简
  化。比如，如果容器类型定义了 value_type 成员类型（标准库容器都会这么做），我们就可
  以有如下实现：
  template<typename C>
  struct ElementT {
  using Type = typename C::value_type;
  };
  这个实现可以是默认实现，它不会排除那些针对没有定义成员类型 value_type 的容器的偏特
  五车书馆
  190
  化实现。
  虽然如此，我们依然建议为类模板的类型参数提供相应的成员类型定义，这样在泛型代码中
  就可以更容易的访问它们（和标准库容器的处理方式类似）。下面的代码体现了这一思想：
  template<typename T1, typename T2, …>
  class X {
  public:
  using … = T1;
  using … = T2;
  …
  };
  那么类型函数的作用体现在什么地方呢？它允许我们根据容器类型参数化一个模板，但是又
  不需要提供代表了元素类型和其它特性的参数。比如，相比于使用
  template<typename T, typename C>
  T sumOfElements (C const& c);
  这一需要显式指定元素类型的模板（sumOfElements<int> list），我们可以定义这样一个模板：
  template<typename C>
  typename ElementT<C>::Type sumOfElements (C const& c);
  其元素类型是通过类型函数得到的。
  注意观察萃取是如何被实现为已有类型的扩充的；也就是说，我们甚至可以为基本类型和封
  闭库的类型定义类型函数。
  在上述情况下，ElementT 被称为萃取类，因为它被用来访问一个已有容器类型的萃取（通
  常而言，在这样一个类中可以有多个萃取）。因此萃取类的功能并不仅限于描述容器参数的
  特性，而是可以描述任意“主参数”的特性。
  为了方便，我们可以伟类型函数创建一个别名模板。比如，我们可以引入：
  template<typename T>
  using ElementType = typename ElementT<T>::Type;
  这可以让 sumOfEkements 的定义变得更加简单：
  template<typename C>
  ElementType<C> sumOfElements (C const& c);
   */

  namespace case1 {
  
  }
  
  /*
转换萃取（Transformation Traits）
  除了可以被用来访问主参数类型的某些特性，萃取还可以被用来做类型转换，比如为某个类
  型添加或移除引用、const 以及 volatile 限制符。
  
  删除引用
  比如，我们可以实现一个 RemoveReferenceT 萃取，用它将引用类型转换成其底层对象或者
  函数的类型，对于非引用类型则保持不变：
    template<typename T>
    struct RemoveReferenceT {
    using Type = T;
    };
    template<typename T>
    struct RemoveReferenceT<T&> {
    using Type = T;
    };
    template<typename T>
    struct RemoveReferenceT<T&&> {
    using Type = T;
    };
    
  同样地，引入一个别名模板可以简化上述萃取的使用：
  template<typename T>
  using RemoveReference = typename RemoveReference<T>::Type;
  当类型是通过一个有时会产生引用类型的构造器获得的时候，从一个类型中删除引用会很有
  意义，比如对于在第 15.6 节介绍的关于函数参数类型 T&&的特殊推断规则。
  C++标准库提供了一个相应的 std::remove_reference<>萃取，详见附录 D.4。
  
  添加引用
  我们也可以给一个已有类型添加左值或者右值引用：
    template<typename T>
    struct AddLValueReferenceT {
    using Type = T&;
    };
    template<typename T>
    using AddLValueReference = typename AddLValueReferenceT<T>::Type;
    template<typename T>
    struct AddRValueReferenceT {
    using Type = T&&;
    };
    
    template<typename T>
    using AddRValueReference = typename AddRValueReferenceT<T>::Type;
  引用折叠的规则在这一依然适用（参见第 15.6 节）。比如对于 AddLValueReference<int &&>，
  返回的类型是 int&，因为我们不需要对它们进行偏特化实现。
  如果我们只实现 AddLValueReferenceT 和 AddRValueReferenceT，而又不对它们进行偏特化的
  话，最方便的别名模板可以被简化成下面这样：
  template<typename T>
  using AddLValueReferenceT = T&;
  template<typename T>
  using AddRValueReferenceT = T&&;
  此时不通过类模板的实例化就可以对其进行实例化（因此称得上是一个轻量级过程）。但是
  这样做是由风险的，因此我们依然希望能够针对特殊的情况对这些模板进行特例化。比如，
  如果适用上述简化实现，那么我们就不能将其用于 void 类型。一些显式的特化实现可以被
  用来处理这些情况：
  template<>
  struct AddLValueReferenceT<void> {
  using Type = void;
  };
  template<>
  struct AddLValueReferenceT<void const> {
  using Type = void const;
  };
  template<>
  struct AddLValueReferenceT<void volatile> {
  using Type = void volatile;
  };
  template<>
  struct AddLValueReferenceT<void const volatile> {
  using Type = void const volatile;
  };
  AddRValueReferenceT 的情况与之类似。
  有了这些偏特化之后，上文中的别名模板必须被实现为类模板的形式（不能适用最简单的那
  种形式），这样才能保证相应的篇特换在需要的时候被正确选取（因为别名模板不能被特化）。
  C++ 标 准 库 中 也 提 供 了 与 之 相 应 的 类 型 萃 取 ： std::add_lvalue_reference<> 和
  std::add_rvalue_reference<>，在附录 D.4 中对它们有专门的介绍。该标准模板也包含了对 void
  类型的特化。
  
  移除限制符
  转换萃取可以分解或者引入任意种类的复合类型，并不仅限于引用。比如，如果一个类型中
  存在 const 限制符，我们可以将其移除：
  template<typename T>
  struct RemoveConstT {
  using Type = T;
  };
  template<typename T>
  struct RemoveConstT<T const> {
  using Type = T;
  };
  template<typename T>
  using RemoveConst = typename RemoveConstT<T>::Type;
  而且，转换萃取可以是多功能的，比如创建一个可以被用来移除 const 和 volatile 的
  RemoveCVT 萃取：
  #include "removeconst.hpp"
  #include "removevolatile.hpp"
  template<typename T>
  struct RemoveCVT : RemoveConstT<typename RemoveVolatileT<T>::Type>
  {
  };
  template<typename T>
  using RemoveCV = typename RemoveCVT<T>::Type;
  RemoveCVT 中有 两个 需要 注意 的地 方。 第一 个需 要注 意的 地方 是， 它同 时使 用了
  RemoveConstT 和相关的 RemoveVolitleT，首先移除类型中可能存在的 volatile，然后将得到
  了类型传递给 RemoveConstT。第二个需要注意的地方是，它没有定义自己的和 RemoveConstT
  中 Type 类似的成员，而是通过使用元函数转发（metafunction forwarding）从 RemoveConstT
  中继承了 Type 成员。这里元函数转发被用来简单的减少 RemoveCVT 中的类型成员。但是，
  即使是对于没有为所有输入都定义了元函数的情况，元函数转发也会很有用，在第 19.4 节
  中会进一步介绍这一技术。
  RemoveCVT 的别名模板可以被进一步简化成：
  template<typename T>
  using RemoveCV = RemoveConst<RemoveVolatile<T>>;
  同样地，这一简化只适用于 RemoveCVT 没有被特化的情况。但是和 AddLValueReference 以
  及 AddRValueReference 的情况不同的是，我们想不出一种对其进行特化的原因。
  C++ 标 准 库 也 提 供 了 与 之 对 应 的 std::remove_volatile<> ， std::remove_const<> ， 以 及
  std::remove_cv<>。在附录 D.4 中有对它们的讨论。
  
  退化（Decay）
  为了使对转换萃取的讨论变得更完整，我们接下来会实现一个模仿了按值传递参数时的类型
  转化行为的萃取。该类型转换继承自 C 语言，这意味着参数类型会发生退化（数组类型退化
  成指针类型，函数类型退化成指向函数的指针类型），而且会删除相应的顶层 const，volatile
  以及引用限制符（因为在解析一个函数调用时，会会忽略掉参数类型中的顶层限制符）。
  下面的程序展现了按值传递的效果，它会打印出经过编译器退化之后的参数类型：
  #include <iostream>
  #include <typeinfo>
  #include <type_traits>
  template<typename T>
  void f(T)
  {}
  template<typename A>
  void printParameterType(void (*)(A))
  {
  std::cout << "Parameter type: " << typeid(A).name() << ’\n’;
  std::cout << "- is int: " <<std::is_same<A,int>::value << ’\n’;
  std::cout << "- is const: " <<std::is_const<A>::value << ’\n’;
  std::cout << "- is pointer: " <<std::is_pointer<A>::value << ’\n’;
  }
  int main()
  {
  printParameterType(&f<int>);
  printParameterType(&f<int const>);
  printParameterType(&f<int[7]>);
  printParameterType(&f<int(int)>);
  }
  在程序的输出中，除了 int 参数保持不变外，其余 int const，int[7]，以及 int(int)参数分别退
  化成了 int，int*，以及 int(*)(int)。
  我们可以实现一个与之功能类似的萃取。为了和 C++标准库中的 std::decay 保持匹配，我们
  称之为 DecayT。它的实现结合了上文中介绍的多种技术。首先我们对非数组、非函数的情
  况进行定义，该情况只需要删除 const 和 volatile 限制符即可：
  template<typename T>
  struct DecayT : RemoveCVT<T>
  {
  };
  然后我们处理数组到指针的退化，这需要用偏特化来处理所有的数组类型（有界和无界数
  组）：
  template<typename T>
  struct DecayT<T[]> {
  using Type = T*;
  };
  template<typename T, std::size_t N>
  struct DecayT<T[N]> {
  using Type = T*;
  };
  最后来处理函数到指针的退化，这需要应对所有的函数类型，不管是什么返回类型以及有多
  数参数。为此，我们适用了变参模板：
  template<typename R, typename… Args>
  struct DecayT<R(Args…)> {
  using Type = R (*)(Args…);
  };
  template<typename R, typename… Args>
  struct DecayT<R(Args…, …)> {
  using Type = R (*)(Args…, …);
  };
  注意，上面第二个偏特化可以匹配任意使用了 C-style 可变参数的函数。下面的例子展示了
  DecayT 主模板以及其全部四种偏特化的使用：
  #include <iostream>
  #include <typeinfo>
  #include <type_traits>
  #include "decay.hpp"
  template<typename T>
  void printDecayedType()
  {
  using A = typename DecayT<T>::Type;
  std::cout << "Parameter type: " << typeid(A).name() << ’\n’;
  std::cout << "- is int: " << std::is_same<A,int>::value << ’\n’;
  std::cout << "- is const: " << std::is_const<A>::value << ’\n’;
  std::cout << "- is pointer: " << std::is_pointer<A>::value << ’\n’;
  }
  int main()
  {
  printDecayedType<int>();
  printDecayedType<int const>();
  printDecayedType<int[7]>();
  printDecayedType<int(int)>();
  }
  
  和往常一样，我们也提供了一个很方便的别名模板：
  template typename T>
  using Decay = typename DecayT<T>::Type;
  
  C++标准库也提供了相应的类型萃取 std::decay<>，在附录 D.4 中有相应的介绍。
   */
  namespace case2 {
    template<typename T>
    struct RemoveReferenceT {
      using Type = T;
    };
    template<typename T>
    struct RemoveReferenceT<T&> {
      using Type = T;
    };
    template<typename T>
    struct RemoveReferenceT<T&&> {
      using Type = T;
    };
    
    template<typename T>
    using RemoveReference = typename RemoveReferenceT<T>::Type;
  
    template<typename T>
    struct RemoveConstT {
      using Type = T;
    };
    template<typename T>
    struct RemoveConstT<T const> {
      using Type = T;
    };
    template<typename T>
    using RemoveConst = typename RemoveConstT<T>::Type;
    
    template <typename T>
    struct RemoveVolatileT {
      using Type = T;
    };
    
    template <typename T>
    struct RemoveVolatileT<T volatile> {
      using Type = T;
    };
  
    /*
  RemoveCVT 中有 两个 需要 注意 的地 方。 第一 个需 要注 意的 地方 是， 它同 时使 用了
    RemoveConstT 和相关的 RemoveVolitleT，首先移除类型中可能存在的 volatile，然后将得到
    了类型传递给 RemoveConstT。第二个需要注意的地方是，它没有定义自己的和 RemoveConstT
    中 Type 类似的成员，而是通过使用元函数转发（metafunction forwarding）从 RemoveConstT
    中继承了 Type 成员。这里元函数转发被用来简单的减少 RemoveCVT 中的类型成员。但是，
    即使是对于没有为所有输入都定义了元函数的情况，元函数转发也会很有用，在第 19.4 节
    中会进一步介绍这一技术。
     */
    template<typename T>
    struct RemoveCVT : RemoveConstT<typename RemoveVolatileT<T>::Type>
    {
    };
    template<typename T>
    using RemoveCV = typename RemoveCVT<T>::Type;
  
    // RemoveCVT 的别名模板可以被进一步简化成：
    template<typename T>
    using RemoveCV2 = RemoveConst<RemoveVolatileT<T>>;
  }
  
  /*
为了使对转换萃取的讨论变得更完整，我们接下来会实现一个模仿了按值传递参数时的类型
  转化行为的萃取。该类型转换继承自 C 语言，这意味着参数类型会发生退化（数组类型退化
  成指针类型，函数类型退化成指向函数的指针类型），而且会删除相应的顶层 const，volatile
  以及引用限制符（因为在解析一个函数调用时，会会忽略掉参数类型中的顶层限制符）。
  下面的程序展现了按值传递的效果，它会打印出经过编译器退化之后的参数类型：
   */
  namespace case3 {
    template<typename T>
    void f(T)
    {
    }
    
    template<typename A>
    void printParameterType(void (*)(A))
    {
      std::cout << "Parameter type: " << typeid(A).name() << endl;
      std::cout << "- is int: " <<std::is_same<A,int>::value << endl;
      std::cout << "- is const: " <<std::is_const<A>::value << endl;
      std::cout << "- is pointer: " <<std::is_pointer<A>::value << endl;
    }
    void test()
    {
      printParameterType(&f<int>);
      printParameterType(&f<int const>);
      printParameterType(&f<int[7]>);
      printParameterType(&f<int(int)>);
    }
  }
  
  namespace case4 {
    template<typename T>
    struct DecayT : case2::RemoveCVT<T>
    {
    };
  
    template<typename T>
    struct DecayT<T[]> {
      using Type = T*;
    };
    
    template<typename T, std::size_t N>
    struct DecayT<T[N]> {
      using Type = T*;
    };
    
    template<typename R, typename... Args>
    struct DecayT<R(Args...)> {
      using Type = R (*)(Args...);
    };
    
    /*
    第二个偏特化可以匹配任意使用了 C-style 可变参数的函数。
     */
    template<typename R, typename... Args>
    struct DecayT<R(Args..., ...)> {
      using Type = R (*)(Args..., ...);
    };
  }
  
  /*
19.3.3 预测型萃取（Predicate Traits）
  到目前为止，我们学习并开发了适用于单个类型的类型函数：给定一个类型，产生另一些相
  关的类型或者常量。但是通常而言，也可以设计基于多个参数的类型函数。这同样会引出另
  外一种特殊的类型萃取--类型预测（产生一个 bool 数值的类型函数）。
  IsSameT
  IsSameT 将判断两个类型是否相同：
  template<typename T1, typename T2>
  struct IsSameT {
  static constexpr bool value = false;
  };
  template<typename T>
  struct IsSameT<T, T> {
  static constexpr bool value = true;
  };
  五车书馆
  197
  这里的主模板说明通常我们传递进来的两个类型是不同的，因此其 value 成员是 false。但是，
  通过使用偏特化，当遇到传递进来的两个相同类型的特殊情况，value 成员就是 true 的。
  比如，如下表达式会判断传递进来的模板参数是否是整型：
  if (IsSameT<T, int>::value) …
  对于产生一个常量的萃取，我们没法为之定义一个别名模板，但是可以为之定义一个扮演可
  相同角色的 constexpr 的变量模板：
  template<typename T1, typename T2>
  constexpr bool isSame = IsSameT<T1, T2>::value;
  C++标准库提供了与之相应的 std::is_same<>，在附录 D.3.3 中有相应的介绍。
  true_type 和 false_type
  通过为可能的输出结果 true 和 false 提供不同的类型，我们可以大大的提高对 IsSameT 的定
  义。事实上，如果我们声明一个 BoolConstant 模板以及两个可能的实例 TrueType 和 FalseType：
  template<bool val>
  struct BoolConstant {
  using Type = BoolConstant<val>;
  static constexpr bool value = val;
  };
  using TrueType = BoolConstant<true>;
  using FalseType = BoolConstant<false>;
  就可以基于两个类型是否匹配，让相应的 IsSameT 分别继承自 TrueType 和 FalseType:
  #include "boolconstant.hpp"
  template<typename T1, typename T2>
  struct IsSameT : FalseType{};
  template<typename T>
  struct IsSameT<T, T> : TrueType{};
  现在 IsSameT<T, int>的返回类型会被隐式的转换成其基类 TrueType 或者 FalseType，这样就不
  仅提供了相应的 value 成员，还允许在编译期间将相应的需求派发到对应的函数实现或者类
  模板的偏特化上。比如：
  #include "issame.hpp"
  #include <iostream>
  template<typename T>
  void fooImpl(T, TrueType)
  {
  std::cout << "fooImpl(T,true) for int called\n";
  五车书馆
  198
  }
  template<typename T>
  void fooImpl(T, FalseType)
  {
  std::cout << "fooImpl(T,false) for other type called\n";
  }
  template<typename T>
  void foo(T t)
  {
  fooImpl(t, IsSameT<T,int>{}); // choose impl. depending on whether T
  is int
  }
  int main()
  {
  foo(42); // calls fooImpl(42, TrueType)
  foo(7.7); // calls fooImpl(42, FalseType)
  }
  这一技术被称为标记派发（tag dispatching），在第 20.2 节有相关介绍。
  注意在 BoolConstant 的实现中还有一个 Type 成员，这样就可以通过它为 IsSameT 引入一个
  别名模板：
  template<typename T>
  using isSame = typename IsSameT<T>::Type;
  这里的别名模板可以和之前的变量模板 isSame 并存。
  通常而言，产生 bool 值的萃取都应该通过从诸如 TrueType 和 FalseType 的类型进行派生来支
  持标记派发。但是为了尽可能的进行泛化，应该只有一个类型代表 true，也应该只有一个类
  型代表 false，而不是让每一个泛型库都为 bool 型常量定义它自己的类型。
  幸运的是，从 C++11 开始 C++ 标准库在<type_traits>中提供了相应的类型：std::true_type 和
  std::false_type。在 C++11 和 C++14 中其定义如下：
  namespace std {
  using true_type = integral_constant<bool, true>;
  using false_type = integral_constant<bool, false>;
  }
  在 C++17 中，其定义如下：
  namespace std {
  using true_type = bool_constant<true>;
  五车书馆
  199
  using false_type = bool_constant<false>;
  }
  其中 bool_constant 的定义如下：
  namespace std {
  template<bool B>
  using bool_constant = integral_constant<bool, B>;
  }
  更多细节请参见附录 D1.1。
  由于这一原因，在本书接下来的部分，我们将直接使用 std::true_type 和 std::false_type，尤
  其是在定义类型预测的时候。
   */
  namespace case5 {
    template<typename T1, typename T2>
    struct IsSameT {
      static constexpr bool value = false;
    };
    template<typename T>
    struct IsSameT<T, T> {
      static constexpr bool value = true;
    };
  
    // 对于产生一个常量的萃取，我们没法为之定义一个别名模板，但是可以为之定义一个扮演可
    //  相同角色的 constexpr 的变量模板：
    template<typename T1, typename T2>
    constexpr bool isSame = IsSameT<T1, T2>::value;
    
  
  }
  
  namespace case6 {
    /*
     * 通过为可能的输出结果 true 和 false 提供不同的类型，我们可以大大的提高对 IsSameT 的定
      义。事实上，如果我们声明一个 BoolConstant 模板以及两个可能的实例 TrueType 和 FalseType：
     */
    template<bool val>
    struct BoolConstant {
      using Type = BoolConstant<val>;
      static constexpr bool value = val;
    };
    
    using TrueType = BoolConstant<true>;
    using FalseType = BoolConstant<false>;
  
    // 就可以基于两个类型是否匹配，让相应的 IsSameT 分别继承自 TrueType 和 FalseType:
    template<typename T1, typename T2>
    struct IsSameT : FalseType{};
    
    template<typename T>
    struct IsSameT<T, T> : TrueType{};
    
    // 测试
    /*
    现在 IsSameT<T, int>的返回类型会被隐式的转换成其基类 TrueType 或者 FalseType，这样就不
    仅提供了相应的 value 成员，还允许在编译期间将相应的需求派发到对应的函数实现或者类模板的偏特化上。
    这一技术被称为标记派发（tag dispatching），在第 20.2 节有相关介绍。
     
     比如：
     */
    template<typename T>
    void fooImpl(T, TrueType)
    {
      std::cout << "fooImpl(T,true) for int called\n";
    }
    
    template<typename T>
    void fooImpl(T, FalseType)
    {
      std::cout << "fooImpl(T,false) for other type called\n";
    }
    template<typename T>
    void foo(T t)
    {
      fooImpl(t, IsSameT<T,int>{}); // choose impl. depending on whether T is int
    }
    
    void test()
    {
      foo(42); // calls fooImpl(42, TrueType)
      foo(7.7); // calls fooImpl(42, FalseType)
    }
  }
  
  /*
19.3.4 返回结果类型萃取（Result Type Traits）
  另一个可以被用来处理多个类型的类型函数的例子是返回值类型萃取。在编写操作符模板的
  时候它们会很有用。为了引出这一概念，我们来写一个可以对两个 Array 容器求和的函数模
  板：
  template<typename T>
  Array<T> operator+ (Array<T> const&, Array<T> const&);
  这看上去很好，但是由于语言本身允许我们对一个 char 型数值和一个整形数值求和，我们
  自然也很希望能够对 Array 也执行这种混合类型（mixed-type）的操作。这样我们就要处理
  该如何决定相关模板的返回值的问题：
  template<typename T1, typename T2>
  Array<???> operator+ (Array<T1> const&, Array<T2> const&);
  除了在第 1.3 节介绍的各种方法外，里一个可以解决上述问题的方式就是返回值类型模板：
  template<typename T1, typename T2>
  Array<typename PlusResultT<T1, T2>::Type>
  operator+ (Array<T1> const&, Array<T2> const&);
  如果有便捷别名模板可用的话，还可以将其写称这样：
  template<typename T1, typename T2>
  Array<PlusResult<T1, T2>>
  operator+ (Array<T1> const&, Array<T2> const&);
  其中的 PlusResultT 萃取会自行判断通过+操作符对两种类型（可能是不同类型）的数值求和
  所得到的类型：
  template<typename T1, typename T2>
  struct PlusResultT {
  using Type = decltype(T1() + T2());
  五车书馆
  200
  };
  template<typename T1, typename T2>
  using PlusResult = typename PlusResultT<T1, T2>::Type;
  这一萃取模板通过使用 decltype 来计算表达式 T1()+T2()的类型，将决定结果类型这一艰巨的
  工作（包括处理类型增进规则（promotion rules）和运算符重载）留给了编译器。
  但是对于我们的例子而言，decltype 却保留了过多的信息（参见第 15.10.2 节中关于 decltype
  行为的介绍）。比如，我们的 PlusResultT 可能会返回一个引用类型，但是我们的 Array 模板
  却很可能不是为引用类型设计的。更为实际的例子是，重载的 operator+可能会返回一个 const
  类型的数值：
  class Integer { … };
  Integer const operator+ (Integer const&, Integer const&);
  对两个 Array<Integer>的值进行求和却得到了一个存储了 Integer const 数值的 Array，这很可
  能不是我们所期望的结果。事实上我们所期望的是将返回值类型中的引用和限制符移除之后
  所得到的类型，正如我们在上一小节所讨论的那样：
  template<typename T1, typename T2>
  Array<RemoveCV<RemoveReference<PlusResult<T1, T2>>>>
  operator+ (Array<T1> const&, Array<T2> const&);
  这一萃取的嵌套形式在模板库中很常见，在元编程中也经常被用到。元编程的内容会在第
  23 章进行介绍。（便捷别名模板在这一类多层级嵌套中会很有用。如果没有它的话，我们
  就必须为每一级嵌套都增加一个 typename 和一个::Type。）
  到目前为止，数组的求和运算符可以正确地计算出对两个元素类型可能不同的 Array 进行求
  和的结果类型。但是上述形式的 PlusResultT 却对元素类型 T1 和 T2 施加了一个我们所不期
  望的限制：由于表达式 T1() + T2()试图对类型 T1 和 T2 的数值进行值初始化，这两个类型必
  须要有可访问的、未被删除的默认构造函数（或者是非 class 类型）。Array 类本身可能并没
  有要求其元素类型可以被进行值初始化，因此这是一个额外的、不必要的限制。
  declval
  好在我们可以很简单的在不需要构造函数的情况下计算+表达式的值，方法就是使用一个可
  以为一个给定类型 T 生成数值的函数。为了这一目的，C++标准提供了 std::declval<>，在第
  11.2.3 节有对其进行介绍。在<utility>中其定义如下：
  namespace std {
  template<typename T>
  add_rvalue_reference_t<T> declval() noexcept;
  }
  五车书馆
  201
  表达式 declval<>可以在不需要使用默认构造函数（或者其它任意操作）的情况下为类型 T
  生成一个值。
  该函数模板被故意设计成未定义的状态，因为我们只希望它被用于 decltype，sizeof 或者其
  它不需要相关定义的上下文中。它有两个很有意思的属性：
  
  对于可引用的类型，其返回类型总是相关类型的右值引用，这能够使 declval 适用于那
  些不能够正常从函数返回的类型，比如抽象类的类型（包含纯虚函数的类型）或者数组
  类型。因此当被用作表达式时，从类型 T 到 T&&的转换对 declval<T>()的行为是没有影
  响的：其结果都是右值（如果 T 是对象类型的话），对于右值引用，其结果之所以不会
  变是因为存在引用塌缩（参见第 15.6 节）。
  
  在 noexcept 异常规则中提到，一个表达式不会因为使用了 declval 而被认成是会抛出异
  常的。当 declval 被用在 noexcept 运算符上下文中时，这一特性会很有帮助（参见第 19.7.2
  节）。
  有了 declval，我们就可以不用在 PlusResultT 中使用值初始化了：
  #include <utility>
  template<typename T1, typename T2>
  struct PlusResultT {
  using Type = decltype(std::declval<T1>() + std::declval<T2>());
  };
  template<typename T1, typename T2>
  using PlusResult = typename PlusResultT<T1, T2>::Type;
  返回值类型萃取提供了一种从特定操作中获取准确的返回值类型的方式，在确定函数模板的
  返回值的类型的时候，它会很有用。
   */
  
  namespace case7 {
    
    template <typename T>
    struct MyArray { };
    
    // 我们来写一个可以对两个 Array 容器求和的函数模板：
    template<typename T>
    MyArray<T> operator+ (MyArray<T> const&, MyArray<T> const&);
  
    // 其中的 PlusResultT 萃取会自行判断通过+操作符对两种类型（可能是不同类型）的数值求和所得到的类型：
    template <typename T1, typename T2>
    struct PlusResultT {
      using Type = decltype(T1{} + T2{});
    };
  
    // 返回值类型模板：
    template<typename T1, typename T2>
    MyArray<typename PlusResultT<T1, T2>::Type>
        operator+ (MyArray<T1> const&, MyArray<T2> const&);
  
    template<typename T1, typename T2>
    using PlusResult = typename PlusResultT<T1, T2>::Type;
    
    /*
    // 如果有便捷别名模板可用的话，还可以将其写称这样：
    template<typename T1, typename T2>
    MyArray<PlusResult<T1, T2>>
        operator+ (MyArray<T1> const&, MyArray<T2> const&);
    */
  
    // 事实上我们所期望的是将返回值类型中的引用和限制符移除之后所得到的类型，正如我们在上一小节所讨论的那样：
    template<typename T1, typename T2>
    MyArray<case2::RemoveCV<case2::RemoveReference<PlusResult<T1, T2>>>>
    operator+ (MyArray<T1> const&, MyArray<T2> const&);
  
    /*
    但是上述形式的 PlusResultT 却对元素类型 T1 和 T2 施加了一个我们所不期
        望的限制：由于表达式 T1() + T2()试图对类型 T1 和 T2 的数值进行值初始化，这两个类型必
        须要有可访问的、未被删除的默认构造函数（或者是非 class 类型）。Array 类本身可能并没
    有要求其元素类型可以被进行值初始化，因此这是一个额外的、不必要的限制。
     */
    
    /*
   declval
      好在我们可以很简单的在不需要构造函数的情况下计算+表达式的值，方法就是使用一个可
      以为一个给定类型 T 生成数值的函数。为了这一目的，C++标准提供了 std::declval<>，
      
        在第11.2.3 节有对其进行介绍。在<utility>中其定义如下：
      namespace std {
        template<typename T>
        add_rvalue_reference_t<T> declval() noexcept;
      }
     */

    //有了 declval，我们就可以不用在 PlusResultT 中使用值初始化了：
    
    template<typename T1, typename T2>
    struct NewPlusResultT {
      using Type = decltype(std::declval<T1>() + std::declval<T2>());
    };
    template<typename T1, typename T2>
    using newPlusResult = typename NewPlusResultT<T1, T2>::Type;
  
  }

  class Tmp {
  public:
  
  };

}

int
//main()
main_Type_Function_19_3()
{
  ch19_3::case3::test();

  return 0;
}