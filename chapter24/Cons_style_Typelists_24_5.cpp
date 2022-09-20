//
// Created by yangfeng on 9/20/2022.
//

#include "../headers.h"

using namespace std;

/*
 24.5 Cons-style Typelists（不完美的类型列表？）
在引入变参模板之前，类型列表通常参照 LISP 的 cons 单元的实现方式，用递归数据结构实
现。每一个 cons 单元包含一个值（列表的 head）和一个嵌套列表，这个嵌套列表可以是另
一个 cons 单元或者一个空的列表 nil。这一思路可以直接在 C++中按照如下方式实现：
class Nil { };
template<typename HeadT, typename TailT = Nil>
五车书馆
339
class Cons {
public:
using Head = HeadT;
using Tail = TailT;
};
一个空的类型列表被记作 Nil，一个包含唯一元素 int 的类型列表则被记作 Cons<int, Nil>，也
可以更简洁的记作 Cons<int>。比较长的列表则需要用到嵌套：
using TwoShort = Cons<short, Cons<unsigned short>>;
任意长度的类型列表则需要用比较深的递归嵌套来实现，虽然手写这么长的一个列表显得很
不明智：
using SignedIntegralTypes = Cons<signed char, Cons<short, Cons<int,
Cons<long, Cons<long long, Nil>>>>>;
要从这样一个 cons-style 的列表中提取第一个元素，只需直接访问其头部元素：
template<typename List>
class FrontT {
public:
using Type = typename List::Head;
};
template<typename List>
using Front = typename FrontT<List>::Type;
向其头部追加以一个元素只需在当前类型列表外面包上一层 Cons 即可：
template<typename List, typename Element>
class PushFrontT {
public:
using Type = Cons<Element, List>;
};
template<typename List, typename Element>
using PushFront = typename PushFrontT<List, Element>::Type;
而如果要删除首元素的话，只需要提取出当前列表的 Tail 即可：
template<typename List>
class PopFrontT {
public:
using Type = typename List::Tail;
};
template<typename List>
using PopFront = typename PopFrontT<List>::Type;
五车书馆
340
至于 IsEmpty 的实现，只需要对 Nil 进行下特例化：
template<typename List>
struct IsEmpty {
static constexpr bool value = false;
};
template<>
struct IsEmpty<Nil> {
static constexpr bool value = true;
};
有了这些操作，就可以使用在第 24.2.7 节中介绍的 InsertionSort 算法了，只是这次是将它用
于 cons-style list（不完美列表）：
template<typename T, typename U>
struct SmallerThanT {
static constexpr bool value = sizeof(T) < sizeof(U);
};
void conslisttest()
{
using ConsList = Cons<int, Cons<char, Cons<short, Cons<double>>>>;
using SortedTypes = InsertionSort<ConsList, SmallerThanT>;using
Expected = Cons<char, Cons<short, Cons<int, Cons<double>>>>;
std::cout << std::is_same<SortedTypes, Expected>::value <<’\n’;
}
正如在 InsertionSort 算法中所见的那样，用 Cons-style 类型列表，可以实现本章中介绍的所
有适用于变参类型列表的算法。事实上，其中一些算法的实现方式和我们操作 cons-style 类
型列表的风格完全一样。但是 cons-style 类型列表的一些缺点还是促使我们更倾向于变参的
版本：首先，嵌套的使用使得长的 cons-style 类型列表在源代码和编译器诊断信息方面即难
以编写又难以阅读。其次，对于变参类型列表，一些算法（包含 PushBack 和 Transform）可
以通过偏特化变的更高效（按照实例的数目计算）。最后，使用变参模板的类型列表能够很
好的适应使用了变参模板的异质容器（比如第 25 章介绍的 tuple 和第 26 章介绍的可识别联
合）。
24.6 后记
类型列表是在 1998 年 C++标准发布之后很快就出现的概念。在[CzarneckiEiseneckerGenProg]
中 Krysztof Czarnecki 和 Ulrich Eisenecker 介绍了受 LISP 启发的、cons-style 的整型常数列表，
虽然没能在此基础上实现向常规类型列表的跨越。
Alexandrescu 在其颇具影响力的图书 Modern C++ Design 中使得类型列表变得流行起来。除
五车书馆
341
此之外，Alexandrescu 还展示了使用类型列表和模板元编程解决一些很有意思的设计问题的
可能，从而让 C++程序员能够比较容易的使用这一技能。
在[AbrahamsGurtovoyMeta]中，Abrahams 和 Gurtovoy 则提供了为元编程所急需的结构，介
绍了类型列表的抽象，类型列表算法，和一些与 C++标准库中名称相似的相关元素：序列，
迭代，算法，和元函数。相关的 Boost.MPL 库，被广泛用来操作类型列表。
 */

namespace ch24_5 {
  
  /*
  在引入变参模板之前，类型列表通常参照 LISP 的 cons 单元的实现方式，用递归数据结构实
    现。每一个 cons 单元包含一个值（列表的 head）和一个嵌套列表，这个嵌套列表可以是另
    一个 cons 单元或者一个空的列表 nil。这一思路可以直接在 C++中按照如下方式实现：
   */
  

  namespace case1 {
    
    class Nil { };
  
    template<typename HeadT, typename TailT = Nil>
    class Cons {
    public:
      using Head = HeadT;
      using Tail = TailT;
    };
  
    // 一个空的类型列表被记作 Nil，一个包含唯一元素 int 的类型列表则被记作 Cons<int, Nil>，
    //   也可以更简洁的记作 Cons<int>。比较长的列表则需要用到嵌套：
    using TwoShort = Cons<short, Cons<unsigned short>>;
  
    // 任意长度的类型列表则需要用比较深的递归嵌套来实现，虽然手写这么长的一个列表显得很不明智：
    using SignedIntegralTypes = Cons<signed char, Cons<short, Cons<int, Cons<long, Cons<long long, Nil>>>>>;
  
    // 要从这样一个 cons-style 的列表中提取第一个元素，只需直接访问其头部元素：
    template<typename List>
    class FrontT {
    public:
      using Type = typename List::Head;
    };
    
    template<typename List>
    using Front = typename FrontT<List>::Type;
    
    // 向其头部追加以一个元素只需在当前类型列表外面包上一层 Cons 即可：
    template<typename List, typename Element>
    class PushFrontT {
    public:
      using Type = Cons<Element, List>;
    };
    
    template<typename List, typename Element>
    using PushFront = typename PushFrontT<List, Element>::Type;
    
    // 而如果要删除首元素的话，只需要提取出当前列表的 Tail 即可：
    
    template<typename List>
    class PopFrontT {
    public:
      using Type = typename List::Tail;
    };
    
    template<typename List>
    using PopFront = typename PopFrontT<List>::Type;
    // 至于 IsEmpty 的实现，只需要对 Nil 进行下特例化：
    
    template<typename List>
    struct IsEmpty {
      static constexpr bool value = false;
    };
    
    template<>
    struct IsEmpty<Nil> {
      static constexpr bool value = true;
    };
    
    // 有了这些操作，就可以使用在第 24.2.7 节中介绍的 InsertionSort 算法了，只是这次是将它用于 cons-style list（不完美列表）：
    template<typename T, typename U>
    struct SmallerThanT {
      static constexpr bool value = sizeof(T) < sizeof(U);
    };
    
    void conslisttest()
    {
      using ConsList = Cons<int, Cons<char, Cons<short, Cons<double>>>>;
      // using SortedTypes = InsertionSort<ConsList, SmallerThanT>;
      using Expected = Cons<char, Cons<short, Cons<int, Cons<double>>>>;
      // std::cout << std::is_same<SortedTypes, Expected>::value << '\n';
    }
    /*
    正如在 InsertionSort 算法中所见的那样，用 Cons-style 类型列表，可以实现本章中介绍的所
      有适用于变参类型列表的算法。事实上，其中一些算法的实现方式和我们操作 cons-style 类
    型列表的风格完全一样。但是 cons-style 类型列表的一些缺点还是促使我们更倾向于变参的
    版本：首先，嵌套的使用使得长的 cons-style 类型列表在源代码和编译器诊断信息方面即难
    以编写又难以阅读。其次，对于变参类型列表，一些算法（包含 PushBack 和 Transform）可
      以通过偏特化变的更高效（按照实例的数目计算）。最后，使用变参模板的类型列表能够很
      好的适应使用了变参模板的异质容器（比如第 25 章介绍的 tuple 和第 26 章介绍的可识别联
      合）。
      */
  
    void test() {
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
main_ch24_5()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch24_5::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch24_5::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch24_5::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch24_5::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch24_5::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}