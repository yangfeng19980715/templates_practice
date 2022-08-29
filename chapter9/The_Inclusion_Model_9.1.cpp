//
// Created by yangfeng on 2022/7/29.
//

// #include "../headers.h"
//#include "myfirst.h"
#include "mysecond.h"

/*
包含模式
  有很多种组织模板源码的方式。
  本章讨论这其中最流行的一种方法：包含模式。
  
  9.1.1 链接错误
    大多数 C 和 C++程序员都会按照如下方式组织代码：
       类和其它类型被放在头文件里。其文件扩展名为.hpp（或者.h, .H, .hh, .hxx）.
       对于全局变量（非 inline）和函数（非 inline），只将其声明放在头文件里，
        定义则被放在 一 个 被 当 作 其 自 身 编 译 单 元 的 文 件 里 。
        这 一 类 文 件 的 扩 展 名 为 .cpp （ 或者.C，.c，.cc，.cxx）。
  
  这样做效果很好：既能够在整个程序中很容易的获得所需类型的定义，同时又避免了链接过程中的重复定义错误。
  
  受这一惯例的影响，刚开始接触模板的程序员通常都会遇到下面这个程序中的错误。
  和处理“常规代码”的情况一样，在头文件中声明模板：
    #ifndef MYFIRST_HPP
    #define MYFIRST_HPP
    
    // declaration of template
    template<typename T>
    void printTypeof (T const&);
    
    #endif //MYFIRST_HPP
    
  其中 printTypeof()是一个简单的辅助函数的声明，它会打印一些类型相关信息。
   而它的具体实现则被放在了一个 CPP 文件中：
    #include <iostream>
    #include <typeinfo>
    #include "myfirst.hpp"
    
    // implementation/definition of template
    template<typename T>
    void printTypeof (T const& x)
    {
      std::cout << typeid(x).name() << ’\n’;
    }
    
  这个函数用 typeid 运算符打印了一个用来描述被传递表达式的类型的字符串。
  该运算符返回一个左值静态类型 std::type_info，它的成员函数 name()可以返回某些表达式的类型。
   C++标准并没有要求 name()必须返回有意义的结果，但是在比较好的 C++实现中，
   它的返回结果应该能够很好的表述传递给 typeid 的参数的类型。
   
  接着在另一个 CPP 文件中使用该模板，它会 include 该模板的头文件：
    #include "myfirst.hpp"
    
    // use of the template
    int main()
    {
      double ice = 3.0;
      printTypeof(ice); // call function template for type double
    }
  编译器很可能会正常编译这个程序，但是链接器则可能会报错说：找不到函数 printTypeof()的定义。
  
  出现这一错误的原因是函数模板 printTypeof()的定义没有被实例化。
  为了实例化一个模板，编译器既需要知道需要实例化哪个函数，也需要知道应该用哪些模板参数来进行实例化。
  
  不幸的是，在上面这个例子中，这两组信息都是被放在别的文件里单独进行编译的。
  因此当编译器遇到对 printTypeof()的调用时，却找不到相对应的函数模板定义来针对 double 类型进行
     实例化，这样编译器只能假设这个函数被定义在别的地方，然后创建一个指向那个函数的引用（会在链接阶段由链接器进行解析）。
   另一方面，在编译器处理 myfirst.cpp 的时候，却没有任何指示让它用某种类型实例化模板。
   
  9.1.2 头文件中的模板
  解决以上问题的方法和处理宏以及 inline 函数的方法一样：将模板定义和模板声明都放在头文件里。
  
  也就是说需要重写 myfirst.hpp，让它包含所有模板声明和模板定义，
  而不再提供 myfirst.cpp文件：
  
    #ifndef MYFIRST_HPP#define MYFIRST_HPP
    #include <iostream>
    #include <typeinfo>
    
    // declaration of template
    template<typename T>
    void printTypeof (T const&);
    
    // implementation/definition of template
    template<typename T>
    void printTypeof (T const& x)
    {
      std::cout << typeid(x).name() << ’\n’;
    }
    #endif //MYFIRST_HPP
    
  这种组织模板相关代码的方法被称为“包含模式”。
  使用这个方法，程序的编译，链接和执行都可以正常进行。
  
  目前有几个问题需要指出。最值得注意的一个是，这一方法将大大增加 include 头文件myfirst.hpp 的成本。
   在这个例子中，成本主要不是由模板自身定义导致的，而是由那些为了使用这个模板而必须包含的头文件导致的，比如<iostream>和<typeinfo>。
   
  由于诸如<iostream>的头文件还会包含一些它们自己的模板，因此这可能会带来额外的数万行的代码。
   这是一个很实际的问题，因为对比较大的程序，它会大大的增加编译时间。
   后面的章节中会涉及到一些可能可以用来解决这一问题的方法，比如预编译头文件（9.2 节）和模板的显式实例化（14.5 节）。
  
  尽管有编译时间的问题，但是除非有更好的方法，我们建议在可能的情况下还是尽量使用这一方式来组织模板代码。
  
  在写作本书的 2017 年，有一个正在准备阶段的机制：modules（C++20已落实），我们会在 17.11 节中介绍相关内容。
   该机制让程序员能够更有逻辑的组织代码，可以让编译器分别编译所有的声明，
   然后在需要的地方高效地、有选择地导入处理之后的声明。
   
  另一个不太明显的问题是，使用 include 方法时，
  非 inline 函数模板和 inline 函数以及宏之间有着明显的不同：
    非 inline 函数模板在被调用的地方不会被展开，而是会被实例化（产生一个函数的新的副本）。
    由于这是一个自动化过程，因此编译器可能会在两个不同的文件中实例化出两份函数的副本，某些链接器在遇到相同函数的两个定义时会报错。
    
  理论上我们不需要关心这一问题：这应该是 C++编译器处理的问题。
  在实践中也是这样，一切运转良好，我们不需要额外做些什么。但是对于比较大的、会创建自己的库的项目，可能会偶尔遇到问题。
  
  在第 14 章中关于实例化方案的讨论，以及对 C++编译系统的研读应该会对解决这一问题有帮助。
  最后需要指出，以上例子中适用于常规函数模板的情况同样适用于类模板的成员函数和静态数据成员，甚至是成员函数模板。
*/

using namespace std;

namespace ch9_1 {
  namespace case1 {
    // ERROR : undefined reference to void printTypeOf<double>(double const &)
    // 函数模板 printTypeof()的定义没有被实例化
    // 为了实例化一个模板，编译器既需要知道需要实例化哪个函数，也需要知道应该用哪些模板参数来进行实例化。
//    void callPrintTypeOf() {
//      double ice = 3.0;
//      printTypeOf(ice);
//    }
  
  }
  
  // 包含模式
  namespace case2 {
    void callPrintTypeOf_2() {
      double ice = 3.0;
      printTypeOf_2(ice);
    }
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
main_the_inclusion_model()
{
  ch9_1::case2::callPrintTypeOf_2();

  return 0;
}
