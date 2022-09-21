//
// Created by yangfeng on 9/20/2022.
//

#include "../headers.h"

using namespace std;

/*
 第 25 章 元组（Tuples）
在本书中，我们经常使用“同质容器”（元素类型相同的），或者类似于数组的类型来说明
模板的功能。这些同质的结构扩展了 C/C++中数组的概念，并且常见于大多数应用之中。C++
（以及 C）也有“异质”的组件：class 或者 struct。本章将会讨论 tuples，它采用了类似于
class 和 struct 的方式来组织数据。比如，一个包含 int，double 和 std::string 的 tuple，和一
个包含 int，double 以及 std::string 类型的成员的 struct 类似，只不过 tuple 中的元素是用位
置信息（比如 0，1，2）索引的，而不是通过名字。元组的位置接口，以及能够容易地从 typelist
构建 tuple 的特性，使得其相比于 struct 更适用于模板元编程技术。
另一种观点是将元组看作在可执行程序中，类型列表的一种表现。比如，类型列表Typelist<int,
double, std::string>，描述了一串包含了 int，double 和 std::string 的、可以在编译期间操作的
类型，而 Tuple<int,double, std::string>则描述了可以在运行期间操作的、对 int，double 和
std::string 的存储。比如下面的程序就创建了这样一个 tuple 的实例：
template<typename... Types>
class Tuple {
... // implementation discussed below
};
Tuple<int, double, std::string> t(17, 3.14, "Hello, World!");
通常会使用模板元编程和 typelist 来创建用于存储数据的 tuple。比如，虽然在上面的程序中
随意地选择了 int，double 和 std::string 作为元素类型，我们也可以用元程序创建一组可被
tuple 存储的类型。
在本章剩余的部分，我们会探讨 Tuple 类模板的相关实现和操作，可以将其看作是 std::tuple
的简化版本。
25.1 基本的元组设计
25.1.1 存储（Storage）
元组包含了对模板参数列表中每一个类型的存储。这部分存储可以通过函数模板 get 进行访
问，对于元组 t，其用法为 get<I>(t)。比如，对于之前例子中的 t，get<0>(t)会返回指向 int 17
的引用，而 get<1>(t)返回的则是指向 double 3.14 的引用。
元组存储的递归实现是基于这样一个思路：一个包含了 N > 0 个元素的元组可以被存储为一
个单独的元素（元组的第一个元素，Head）和一个包含了剩余 N-1 个元素（Tail）的元组，
对于元素为空的元组，只需当作特例处理即可。因此一个包含了三个元素的元组 Tuple<int,
五车书馆
343
double, std::string> 可以被存储为一个 int 和一个 Tuple<double, std::string>。
这个包含两个元素的元组又可以被存储为一个 double 和一个 Tuple<std::string>，这个
只包含一个元素的元组又可以被存储为一个 std::string 和一个空的元组 Tuple<>。事实上，在
类型列表算法的泛型版本中也使用了相同的递归分解过程，而且实际递归元组的存储实现也
以类似的方式展开：
template<typename... Types>
class Tuple;
// recursive case:
template<typename Head, typename... Tail>
class Tuple<Head, Tail...>
{
private:
Head head;
Tuple<Tail...> tail;
public:
// constructors:
Tuple() {
}
Tuple(Head const& head, Tuple<Tail...> const& tail): head(head),
tail(tail) {
}
...
Head& getHead() { return head; }
Head const& getHead() const { return head; }
Tuple<Tail...>& getTail() { return tail; }
Tuple<Tail...> const& getTail() const { return tail; }
};
// basis case:
template<>
class Tuple<> {
// no storage required
};
在递归情况下，Tuple 的实例包含一个存储了列表首元素的 head，以及一个存储了列表剩余
元素的 tail。基本情况则是一个没有存储内容的简单的空元组。
而函数模板 get 则会通过遍历这个递归的结构来提取所需要的元素：
// recursive case:
template<unsigned N>
struct TupleGet {
五车书馆
344
template<typename Head, typename... Tail>
static auto apply(Tuple<Head, Tail...> const& t) {
return TupleGet<N-1>::apply(t.getTail());
}
};
// basis case:
template<>
struct TupleGet<0> {
template<typename Head, typename... Tail>
static Head const& apply(Tuple<Head, Tail...> const& t) {
return t.getHead();
}
};
template<unsigned N, typename... Types>
auto get(Tuple<Types...> const& t) {
return TupleGet<N>::apply(t);
}
注意，这里的函数模板 get 只是封装了一个简单的对 TupleGet 的静态成员函数调用。在不能
对函数模板进行部分特例化的情况下（参见 17.3 节），这是一个有效的变通方法，在这里
针对非类型模板参数 N 进行了特例化。在 N > 0 的递归情况下，静态成员函数 apply()会提取
出当前 tuple 的 tail，递减 N，然后继续递归地在 tail 中查找所需元素。对于 N=0 的基本情况，
apply()会返回当前 tuple 的 head，并结束递归。
25.1.2 构造
除了前面已经定义的构造函数：
Tuple() {
}
Tuple(Head const& head, Tuple<Tail...> const& tail)
: head(head), tail(tail)
{
}
为了让元组的使用更方便，还应该允许用一组相互独立的值（每一个值对应元组中的一个元
素）或者另一个元组来构造一个新的元组。从一组独立的值去拷贝构造一个元组，会用第一
个数值去初始化元组的 head，而将剩余的值传递给 tail：
Tuple(Head const& head, Tail const&... tail)
: head(head), tail(tail...)
{
五车书馆
345
}
这样就可以像下面这样初始化一个元组了：
Tuple<int, double, std::string> t(17, 3.14, "Hello, World!");
不过这并不是最通用的接口：用户可能会希望用移动构造（move-construct）来初始化元组
的一些（可能不是全部）元素，或者用一个类型不相同的数值来初始化元组的某个元素。因
此我们需要用完美转发（参见 15.6.3 节）来初始化元组：
template<typename VHead, typename... VTail>
Tuple(VHead&& vhead, VTail&&... vtail)
: head(std::forward<VHead>(vhead)), tail(std::forward<VTail>(vtail)...)
{
}
下面的这个实现则允许用一个元组去构建另一个元组：
template<typename VHead, typename... VTail>
Tuple(Tuple<VHead, VTail...> const& other)
: head(other.getHead()), tail(other.getTail())
{ }
但是这个构造函数不适用于类型转换：给定上文中的 t，试图用它去创建一个元素之间类型
兼容的元组会遇到错误：
// ERROR: no conversion from Tuple<int, double, string> to long
Tuple<long int, long double, std::string> t2(t);
这是因为上面这个调用，会更匹配用一组数值去初始化一个元组的构造函数模板，而不是用
一个元组去初始化另一个元组的构造函数模板。为了解决这一问题，就需要用到 6.3 节介绍
的 std::enable_if<>，在 tail 的长度与预期不同的时候就禁用相关模板：
template<typename VHead, typename... VTail, typename = std::enable_if_t<sizeof...
(VTail)==sizeof... (Tail)>>
Tuple(VHead&& vhead, VTail&&... vtail)
: head(std::forward<VHead>(vhead)), tail(std::forward<VTail>(vtail)...)
{ }
template<typename VHead, typename... VTail, typename = std::enable_if_t<sizeof...
(VTail)==sizeof... (Tail)>>
Tuple(Tuple<VHead, VTail...> const& other)
: head(other.getHead()), tail(other.getTail()) { }
你可以在 tuples/tuple.h 中找到所有的构造函数声明。
函数模板 makeTuple()会通过类型推断来决定所生成元组中元素的类型，这使得用一组数值
创建一个元组变得更加简单：
template<typename... Types>
五车书馆
346
auto makeTuple(Types&&... elems)
{
return Tuple<std::decay_t<Types>...>(std::forward<Types> (elems)...);
}
这里再一次将 std::decay<>和完美转发一起使用，这会将字符串常量和裸数组转换成指针，
并去除元素的 const 和引用属性。比如：
makeTuple(17, 3.14, "Hello, World!")
生成的元组的类型是：
Tuple<int, double, char const*>
 */

namespace ch25_1 {
  
  namespace case1 {
    
    /*
    template<typename... Types>
    class Tuple {
      // ... // implementation discussed below
    };
    Tuple<int, double, std::string> t(17, 3.14, "Hello, World!");
     */
    
    /*
      元组包含了对模板参数列表中每一个类型的存储。
      这部分存储可以通过函数模板 get 进行访问，对于元组 t，其用法为 get<I>(t)。
      比如，对于之前例子中的 t，get<0>(t)会返回指向 int 17 的引用，而 get<1>(t)返回的则是指向 double 3.14 的引用。
     */
    
    template<typename... Types>
    class Tuple;
  
    // basis case:
    template<>
    class Tuple<> {
      // no storage required
    };

    // recursive case:
    template<typename Head, typename... Tail>
    class Tuple<Head, Tail...> {
    private:
      Head head;
      Tuple<Tail...> tail;
      
    public:
      // constructors:
      Tuple() { }
      
      Tuple(Head const &head, Tuple<Tail...> const &tail) : head(head), tail(tail) { }
  
      /*
      为了让元组的使用更方便，还应该允许用一组相互独立的值（每一个值对应元组中的一个元
        素）或者另一个元组来构造一个新的元组。从一组独立的值去拷贝构造一个元组，会用第一
        个数值去初始化元组的 head，而将剩余的值传递给 tail：
        */
      Tuple(Head const& head, Tail const&... tail) : head(head), tail(tail...) { }
  
      // 这样就可以像下面这样初始化一个元组了：
      //Tuple<int, double, std::string> t(17, 3.14, "Hello, World!");
      
      /*
      不过这并不是最通用的接口：用户可能会希望用移动构造（move-construct）来初始化元组
        的一些（可能不是全部）元素，或者用一个类型不相同的数值来初始化元组的某个元素。因
        此我们需要用完美转发（参见 15.6.3 节）来初始化元组：
        */
      // template<typename VHead, typename... VTail>
      // Tuple(VHead&& vhead, VTail&&... vtail) : head(std::forward<VHead>(vhead)), tail(std::forward<VTail>(vtail)...) { }
  
      // 下面的这个实现则允许用一个元组去构建另一个元组：
      // template<typename VHead, typename... VTail>
      // Tuple(Tuple<VHead, VTail...> const& other) : head(other.getHead()), tail(other.getTail()) { }
      
      // 但是这个构造函数不适用于类型转换：给定上文中的 t，试图用它去创建一个元素之间类型兼容的元组会遇到错误：
      // ERROR: no conversion from Tuple<int, double, string> to long
      // Tuple<long int, long double, std::string> t2(t);
  
      /*
  这是因为上面这个调用，会更匹配用一组数值去初始化一个元组的构造函数模板，而不是用
    一个元组去初始化另一个元组的构造函数模板。为了解决这一问题，就需要用到 6.3 节介绍
    的 std::enable_if<>，在 tail 的长度与预期不同的时候就禁用相关模板：
       */
      template<typename VHead, typename... VTail, typename = std::enable_if_t<sizeof... (VTail) == sizeof... (Tail)>>
      Tuple(VHead &&vhead, VTail &&... vtail) : head(std::forward<VHead>(vhead)), tail(std::forward<VTail>(vtail)...) {}
  
      template<typename VHead, typename... VTail, typename = std::enable_if_t<sizeof... (VTail) == sizeof... (Tail)>>
      Tuple(Tuple<VHead, VTail...> const &other) : head(other.getHead()), tail(other.getTail()) {}
      
      Head &getHead() { return head; }
      
      Head const &getHead() const { return head; }
      
      Tuple<Tail...> &getTail() { return tail; }
      
      Tuple<Tail...> const &getTail() const { return tail; }
    };

  
    /*
    在递归情况下，Tuple 的实例包含一个存储了列表首元素的 head，以及一个存储了列表剩余
      元素的 tail。基本情况则是一个没有存储内容的简单的空元组。
    而函数模板 get 则会通过遍历这个递归的结构来提取所需要的元素：
     */
    // recursive case:
    template<unsigned N>
    struct TupleGet {
      
      template<typename Head, typename... Tail>
      static auto apply(Tuple<Head, Tail...> const& t) {
        return TupleGet<N-1>::apply(t.getTail());
      }
    };
    
    // basis case:
    template<>
    struct TupleGet<0> {
      
      template<typename Head, typename... Tail>
      static Head const& apply(Tuple<Head, Tail...> const& t) {
        return t.getHead();
      }
    };
    
    template<unsigned N, typename... Types>
    auto get(Tuple<Types...> const& t) {
      return TupleGet<N>::apply(t);
    }
    
    /*
      注意，这里的函数模板 get 只是封装了一个简单的对 TupleGet 的静态成员函数调用。在不能
      对函数模板进行部分特例化的情况下（参见 17.3 节），这是一个有效的变通方法，在这里
      针对非类型模板参数 N 进行了特例化。在 N > 0 的递归情况下，静态成员函数 apply()会提取
      出当前 tuple 的 tail，递减 N，然后继续递归地在 tail 中查找所需元素。对于 N=0 的基本情况，
      apply()会返回当前 tuple 的 head，并结束递归。
     */

    /*
    你可以在 tuples/tuple.h 中找到所有的构造函数声明。
    函数模板 makeTuple()会通过类型推断来决定所生成元组中元素的类型，这使得用一组数值
      创建一个元组变得更加简单：
      */
    template<typename... Types>
    auto makeTuple(Types&&... elems) {
      return Tuple<std::decay_t<Types>...>(std::forward<Types> (elems)...);
    }
    
    // 这里再一次将 std::decay<>和完美转发一起使用，这会将字符串常量和裸数组转换成指针，并去除元素的 const 和引用属性。
    // 比如：
    // 生成的元组的类型是： Tuple<int, double, char const*>
    // Tuple<int, double, char const*> var = makeTuple(17, 3.14, "Hello, World!");
    
    /*
      25.2.1 比较
元组是包含了其它数值的结构化类型。为了比较两个元组，就需要比较它们的元素。因此可
以像下面这样，定义一种能够逐个比较两个元组中元素的 operator==：
     */
    // basis case:
    bool operator==(Tuple<> const&, Tuple<> const&) {
      // empty tuples are always equivalentreturn true;
      return true;
    }
    
    // recursive case:
    template< typename Head1, typename... Tail1,
              typename Head2, typename... Tail2,
              typename = std::enable_if_t<sizeof...(Tail1) == sizeof...(Tail2)>>
    bool operator==(Tuple<Head1, Tail1...> const &lhs, Tuple<Head2, Tail2...> const &rhs) {
      return lhs.getHead() == rhs.getHead() &&
             lhs.getTail() == rhs.getTail();
    }
    
    /*
     和其它适用于类型列表和元组的算法类似，逐元素的比较两个元组，会先比较首元素，然后递归地比较剩余的元素，
     最终会调用 operator 的基本情况结束递归。运算符!=，<，>，以及>=的实现方式都与之类似。
     */
    
    /*
     25.2.2 输出
        贯穿本章始终，我们一直都在创建新的元组类型，因此最好能够在执行程序的时候看到这些元组。
        下面的 operator<<运算符会打印那些元素类型可以被打印的元组：
     */
    void printTuple(std::ostream& strm, Tuple<> const&, bool isFirst = true) {
      strm << ( isFirst ? '(' : ')' );
    }
    
    template<typename Head, typename... Tail>
    void printTuple(std::ostream& strm, Tuple<Head, Tail...> const& t, bool isFirst = true) {
      strm << (isFirst ? "(" : ", ");
      strm << t.getHead();
      printTuple(strm, t.getTail(), false);
    }
  
    template<typename ... Types>
    std::ostream &operator<<(std::ostream &strm, Tuple<Types...> const &t) {
      printTuple(strm, t);
      return strm;
    }
    
    /*
      25.3.1 将元组用作类型列表
      如果我们忽略掉 Tuple 模板在运行期间的相关部分，可以发现它在结构上和第 24 章介绍的
      Typelist 完全一样：都接受任意数量的模板类型参数。事实上，通过使用一些部分特例化，
      可以将 Tuple 变成一个功能完整的 Typelist
     */
    template<typename T>
    struct IsEmpty;
    
    // determine whether the tuple is empty:
    template<>
    struct IsEmpty<Tuple<>> {
      static constexpr bool value = true;
    };
    
    template<typename Head, typename... Tail>
    class FrontT ;
    
    
    // extract front element:
    template<typename Head, typename... Tail>
    class FrontT<Tuple<Head, Tail...>> {
    public:
        using Type = Head;
    };
  
    template<typename Head, typename... Tail>
    class PopFrontT;
    
    // remove front element:
    template<typename Head, typename... Tail>
    class PopFrontT<Tuple<Head, Tail...>> {
    public:
      using Type = Tuple<Tail...>;
    };
    
    template <typename Head, typename ...Tail>
    using PopFront = typename PopFrontT<Head, Tail...>::Type;
  
    template<typename Types, typename Element>
    class PushFrontT;
    
    // add element to the front:
    template<typename... Types, typename Element>
    class PushFrontT<Tuple<Types...>, Element> {
    public:
      using Type = Tuple<Element, Types...>;
    };
  
    template<typename Types, typename Element>
    class PushBackT;
    
    // add element to the back:
    template<typename... Types, typename Element>
    class PushBackT<Tuple<Types...>, Element> {
    public:
      using Type = Tuple<Types..., Element>;
    };
    
    template <typename Types, typename Element>
    using PushBack = typename PushBackT<Types, Element>::Type;
  
    void test_1() {
      // 现在，所有在第 24 章开发的 typlist 算法都既适用于 Tuple 也适用于 Typelist，这样就可以很方便的处理元组的类型了。比如：
      Tuple<int, double, std::string> t1(17, 3.14, "Hello, World!");
      using T2 = PopFront<PushBack<decltype(t1), bool>>;
      T2 t2(get<1>(t1), get<2>(t1), true);
      // std::cout << t2; // 会打印出： (3.14, Hello, World!, 1)
      // 很快就会看到，将 typelist 算法用于 tuple，通常是为了确定 tuple 算法返回值的类型。
      std::cout << makeTuple(1, 2.5, std::string("hello")) << '\n';
    }
    
    void test() {
      test_1();
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
//main_ch25_1()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch25_1::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch25_1::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch25_1::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch25_1::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch25_1::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;
  
  return 0;
}