//
// Created by yangfeng on 8/31/2022.
//

#include "../headers.h"

using namespace std;

/*
21.3 Mixins（混合？）
考虑一个包含了一组点的简单 Polygon 类：
class Point
{
public:
double x, y;
五车书馆
285
Point() : x(0.0), y(0.0) { }
Point(double x, double y) : x(x), y(y) { }
};
class Polygon
{
private:
std::vector<Point> points;
public:
... //public operations
};
如果可以扩展与每个 Point 相关联的一组信息的话（比如包含特定应用中每个点的颜色，或
者给每个点加个标签），那么 Polygon 类将变得更为实用。实现该扩展的一种方式是用点的
类型对 Polygon 进行参数化：
template<typename P>
class Polygon
{
private:
std::vector<P> points;
public:
... //public operations
};
用户可以通过继承创建与 Point 类似，但是包含了特定应用所需数据，并且提供了与 Point
相同的接口的类型：
class LabeledPoint : public Point
{
public:
std::string label;
LabeledPoint() : Point(), label("") { }
LabeledPoint(double x, double y) : Point(x, y), label("") {
}
};
这一实现方式有其自身的缺点。比如，首先需要将 Point 类型暴露给用户，这样用户才能从
它派生出自己的类型。而且 LablePoint 的作者也需要格外小心地提供与 Point 完全一样的接
口（比如，继承或者提供所有与 Point 相同的构造函数），否则在 Polygon 中使用 LabledPoint
的时候会遇到问题。这一问题在 Point 随 Polygon 模板版本发生变化时将会变得更加严重：
如果给 Point 新增一个构造函数，就需要去更新所有的派生类。
Mixins 是另一种可以客制化一个类型的行为但是不需要从其进行继承的方法。事实上，Mixins
反转了常规的继承方向，因为新的类型被作为类模板的基类“混合进”了继承层级中，而不
是被创建为一个新的派生类。这一方式允许在引入新的数据成员以及某些操作的时候，不需
五车书馆
286
要去复制相关接口。
一个支持了 mixins 的类模板通常会接受一组任意数量的 class，并从之进行派生：
template<typename... Mixins>
class Point : public Mixins...
{
public:
double x, y;
Point() : Mixins()..., x(0.0), y(0.0) { }
Point(double x, double y) : Mixins()..., x(x), y(y) { }
};
现在，我们就可以通过将一个包含了 label 的基类“混合进来（mix in）”来生成一个
LabledPoint：
class Label
{
public:
std::string label;
Label() : label("") { }
};
using LabeledPoint = Point<Label>;
甚至是“mix in”几个基类：
class Color
{
public:
unsigned char red = 0, green = 0, blue = 0;
};
using MyPoint = Point<Label, Color>;
有了这个基于 mixin 的 Point，就可以在不改变其接口的情况下很容易的为 Point 引入额外的
信息，因此 Polygon 的使用和维护也将变得相对简单一些。为了访问相关数据和接口，用户
只需进行从 Point 到它们的 mixin 类型（Label 或者 Color）之间的隐式转化即可。而且，通
过提供给 Polygon 类模板的 mixins，Point 类甚至可以被完全隐藏：
template<typename... Mixins>
class Polygon
{
private:
std::vector<Point<Mixins...>> points;
public:
... //public operations
};
五车书馆
287
当需要对模板进行少量客制化的时候，Mixins 会很有用，比如在需要用用户指定的数据去装
饰内部存储的对象时，使用 mixins 就不需要将内部数据类型和接口暴露出来并写进文档。
21.3.1 Curious Mixins
在和第 21.2 节介绍的 CRTP 一起使用的时候，Mixins 会变得更强大。此时每一个 mixins 都是
一个以派生类为模板参数的类模板，这样就允许对派生类做额外的客制化。一个 CRTP-mixin
版本的 Point 可以被下称下面这样：
template<template<typename>... Mixins>
class Point : public Mixins<Point>...
{
public:
double x, y;
Point() : Mixins<Point>()..., x(0.0), y(0.0) { }
Point(double x, double y) : Mixins<Point>()..., x(x), y(y) { }
};
这一实现方式需要对那些将要被混合进来（mix in）的类做一些额外的工作，因此诸如 Label
和 Color 一类的 class 需要被调整成类模板。但是，现在这些被混合进来的 class 的行为可以
基于其降要被混合进的派生类进行调整。比如，我们可以将前述的 ObjectCounter 模板混合
进 Point，这样就可以统计在 Polygon 中创建的点的数目。
21.3.2 Parameterized Virtuality（虚拟性的参数化）
Minxins 还允许我们去间接的参数化派生类的其它特性，比如成员函数的虚拟性。下面的简
单例子展示了这一令人称奇的技术：
#include <iostream>
class NotVirtual {
};
class Virtual {
public:
virtual void foo() {
}
};
template<typename... Mixins>
class Base : public Mixins...
{
public:
五车书馆
288
// the virtuality of foo() depends on its declaration
// (if any) in the base classes Mixins...
void foo() {
std::cout << "Base::foo()" << ’\n’;
}
};
template<typename... Mixins>
class Derived : public Base<Mixins...> {
public:
void foo() {
std::cout << "Derived::foo()" << ’\n’;
}
};
int main()
{
Base<NotVirtual>* p1 = new Derived<NotVirtual>;
p1->foo(); // calls Base::foo()
Base<Virtual>* p2 = new Derived<Virtual>;
p2->foo(); // calls Derived::foo()
}
该技术提供了这样一种工具，使用它可以设计出一个既可以用来实例化具体的类，也可以通
过继承对其进行扩展的类模板。但是，要获得一个可以为某些更为特化的功能产生一个更好
的基类的类，仅仅是针对某些成员函数进行虚拟化还是不够的。这一类开发方法需要更为基
础的设计决策。更为实际的做法是设计两个不同的工具（类或者类模板层级），而不是将它
们集成进一个模板层级。
 */

namespace ch21_3 {

  namespace case1 {
  
    class Point
    {
    public:
      double x, y;
      Point() : x(0.0), y(0.0) { }
      Point(double x, double y) : x(x), y(y) { }
    };
    
    class Old_Polygon
    {
    private:
      std::vector<Point> points;
    public:
       //public operations
    };
    
    /*
    如果可以扩展与每个 Point 相关联的一组信息的话（比如包含特定应用中每个点的颜色，或
者给每个点加个标签），那么 Polygon 类将变得更为实用。实现该扩展的一种方式是用点的
类型对 Polygon 进行参数化：
     */
    
    template<typename P>
    class Polygon
    {
    private:
      std::vector<P> points;
    public:
       //public operations
    };
  
    // 用户可以通过继承创建与 Point 类似，但是包含了特定应用所需数据，并且提供了与 Point 相同的接口的类型：
    class LabeledPoint : public Point
    {
    public:
      std::string label;
      LabeledPoint() : Point(), label("") { }
      LabeledPoint(double x, double y) : Point(x, y), label("") {
      }
    };
  
    void test() {
      cout << "hello, world" << endl;
    }
    
  }
  
  /*
  这一实现方式有其自身的缺点。比如，首先需要将 Point 类型暴露给用户，这样用户才能从
    它派生出自己的类型。而且 LablePoint 的作者也需要格外小心地提供与 Point 完全一样的接
    口（比如，继承或者提供所有与 Point 相同的构造函数），否则在 Polygon 中使用 LabledPoint
  的时候会遇到问题。这一问题在 Point 随 Polygon 模板版本发生变化时将会变得更加严重：
  如果给 Point 新增一个构造函数，就需要去更新所有的派生类。
  Mixins 是另一种可以客制化一个类型的行为但是不需要从其进行继承的方法。事实上，Mixins
    反转了常规的继承方向，因为新的类型被作为类模板的基类“混合进”了继承层级中，而不
    是被创建为一个新的派生类。这一方式允许在引入新的数据成员以及某些操作的时候，不需
  要去复制相关接口。
  一个支持了 mixins 的类模板通常会接受一组任意数量的 class，并从之进行派生：
   */
  
  namespace case2 {
    template<typename... Mixins>
    class Point : public Mixins...
    {
      public:
      double x, y;
      Point() : Mixins()..., x(0.0), y(0.0) { }
      Point(double x, double y) : Mixins()..., x(x), y(y) { }
    };
  
  
    /* 现在，我们就可以通过将一个包含了 label 的基类“混合进来（mix in）”来生成一个LabledPoint：  */
    class Label
    {
    public:
      std::string label;
      Label() : label("") { }
    };
    
    using LabeledPoint = Point<Label>;
    
    /* 甚至是“mix in”几个基类： */
    class Color
    {
    public:
      unsigned char red = 0, green = 0, blue = 0;
    };
    
    using MyPoint = Point<Label, Color>;
    
    /*
     有了这个基于 mixin 的 Point，就可以在不改变其接口的情况下很容易的为 Point 引入额外的
信息，因此 Polygon 的使用和维护也将变得相对简单一些。为了访问相关数据和接口，用户
只需进行从 Point 到它们的 mixin 类型（Label 或者 Color）之间的隐式转化即可。而且，通
过提供给 Polygon 类模板的 mixins，Point 类甚至可以被完全隐藏：
     */

    template<typename... Mixins>
    class Polygon
    {
    private:
      std::vector<Point<Mixins...>> points;
    public:
       //public operations
    };
    
    /*
     当需要对模板进行少量客制化的时候，Mixins 会很有用，比如在需要用用户指定的数据去装
饰内部存储的对象时，使用 mixins 就不需要将内部数据类型和接口暴露出来并写进文档。
     */
  
    void test() {
    
    }
    
  }
  
  /*
   21.3.1 Curious Mixins
在和第 21.2 节介绍的 CRTP 一起使用的时候，Mixins 会变得更强大。此时每一个 mixins 都是
一个以派生类为模板参数的类模板，这样就允许对派生类做额外的客制化。一个 CRTP-mixin
版本的 Point 可以被下称下面这样：
template<template<typename>... Mixins>
class Point : public Mixins<Point>...
{
public:
double x, y;
Point() : Mixins<Point>()..., x(0.0), y(0.0) { }
Point(double x, double y) : Mixins<Point>()..., x(x), y(y) { }
};
这一实现方式需要对那些将要被混合进来（mix in）的类做一些额外的工作，因此诸如 Label
和 Color 一类的 class 需要被调整成类模板。但是，现在这些被混合进来的 class 的行为可以
基于其降要被混合进的派生类进行调整。比如，我们可以将前述的 ObjectCounter 模板混合
进 Point，这样就可以统计在 Polygon 中创建的点的数目。
   */
  
  namespace case3 {
    
    template<
      template<typename>
      typename ...  Mixins
      >
    class Point : public Mixins<Point<Mixins...>>...
    {
      public:
      double x, y;
      Point() : Mixins<Point>()..., x(0.0), y(0.0) { }
      Point(double x, double y) : Mixins<Point>()..., x(x), y(y) { }
    };
    
    /*
     这一实现方式需要对那些将要被混合进来（mix in）的类做一些额外的工作，因此诸如 Label
和 Color 一类的 class 需要被调整成类模板。但是，现在这些被混合进来的 class 的行为可以
基于其降要被混合进的派生类进行调整。比如，我们可以将前述的 ObjectCounter 模板混合
进 Point，这样就可以统计在 Polygon 中创建的点的数目。
     */
      
    void test() {
    
    }
    
  }
  
  namespace case4 {
    
    /*
     21.3.2 Parameterized Virtuality（虚拟性的参数化）
Minxins 还允许我们去间接的参数化派生类的其它特性，比如成员函数的虚拟性。下面的简
单例子展示了这一令人称奇的技术：
     */
    class NotVirtual { };
    
    class Virtual {
    public:
      virtual void foo() {
      }
    };
    
    template<typename... Mixins>
    class Base : public Mixins...
    {
    public:
      // the virtuality of foo() depends on its declaration
      // (if any) in the base classes Mixins…
      void foo() { std::cout << "Base::foo()" << endl; }
    };
    
    template<typename... Mixins>
    class Derived : public Base<Mixins...> {
    public:
      void foo() { std::cout << "Derived::foo()" << endl; }
    };
    
    void test()
    {
      Base<NotVirtual>* p1 = new Derived<NotVirtual>;
      p1->foo(); // calls Base::foo()
      Base<Virtual>* p2 = new Derived<Virtual>;
      p2->foo(); // calls Derived::foo()
    }
    
    /*
    该技术提供了这样一种工具，使用它可以设计出一个既可以用来实例化具体的类，也可以通
过继承对其进行扩展的类模板。但是，要获得一个可以为某些更为特化的功能产生一个更好
的基类的类，仅仅是针对某些成员函数进行虚拟化还是不够的。这一类开发方法需要更为基
础的设计决策。更为实际的做法是设计两个不同的工具（类或者类模板层级），而不是将它
们集成进一个模板层级。
     */
    
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
main_ch21_3()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch21_3::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch21_3::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch21_3::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch21_3::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch21_3::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}