//
// Created by yangfeng on 9/25/2022.
//

#include "../headers.h"

using namespace std;



namespace ch25_5 {

  namespace case1 {
    /*
    25.5 元组的优化
      元组是一种基础的、潜在用途广泛的异质容器。因此有必要考虑下该怎么在运行期（存储和
      执行时间）和编译期（实例化的数量）对其进行优化。本节将介绍一些适用于上文中实现的
      元组的特定优化方案。
      
    25.5.1 元组和 EBCO
    我们实现的元组，其存储方式所需要的存储空间，要比其严格意义上所需要的存储空间多。
    其中一个问题是，tail 成员最终会是一个空的数值（因为所有非空的元组都会以一个空的元
      组作为结束），而任意数据成员又总会至少占用一个字节的内存（参见 21.1 节）。
    为了提高元组的存储效率，可以使用第 21.1 节介绍的空基类优化（EBCO，empty base class
      optimization），让元组继承自一个尾元组（tail tuple），而不是将尾元组作为一个成员。比
      如：
      */
    /*
    
    template <typename ... Types>
    class Tuple;
    
    // recursive case:
    template<typename Head, typename... Tail>
    class Tuple<Head, Tail...> : private Tuple<Tail...> {
    private:
      Head head;
      
    public:
      Head &getHead() { return head; }
  
      Head const &getHead() const { return head; }
  
      Tuple<Tail...> &getTail() { return *this; }
  
      Tuple<Tail...> const &getTail() const { return *this; }
    };
     */
    
    /*
      这和第 21.1.2 节中的 BaseMemberPair 使用的优化方式一致。不幸的是，这种方式有其副作
用，就是颠倒了元组元素在构造函数中被初始化的顺序。在之前的实现中，head 成员在 tail
成员前面，因此 head 总是会先被初始化。在新的实现方式中，tail 则是以基类的形式存在，
因此它会在 head 成员之前被初始化。
这一问题可以通过将 head 成员放入其自身的基类中，并让这个基类在基类列表中排在 tail
的前面来解决。该方案的一个直接实现方式是，引入一个用来封装各种元素类型的 TupleElt
模板，并让 Tuple 继承自它：
     */
  
    /*
    template<typename... Types>
    class Tuple;
    
    template<typename T>
    class TupleElt {
      T value;
      
    public:
      TupleElt() = default;
      template<typename U>
      TupleElt(U&& other) : value(std::forward<U>(other)) { }
      
      T& get() { return value; }
      
      T const& get() const { return value; }
    };
    
    // recursive case:
    template<typename Head, typename... Tail>
    class Tuple<Head, Tail...> : private TupleElt<Head>, private Tuple<Tail...> {
    public:
      Head& getHead() {
        // potentially ambiguous
        return static_cast<TupleElt<Head> *>(this)->get();
      }
      Head const& getHead() const {
        // potentially ambiguous
        return static_cast<TupleElt<Head> const*>(this)->get();
      }
      Tuple<Tail...>& getTail() { return *this; }
      Tuple<Tail...> const& getTail() const { return *this; }
    };
    
    // basis case:
    template<>
    class Tuple<> {
      // no storage required
    };
    */
  
    /*
      虽然这一方式解决了元素初始化顺序的问题，但是却引入了一个更糟糕的问题：如果一个元
组包含两个类型相同的元素（比如 Tuple<int, int>），我们将不再能够从中提取元素，因为
此时从 Tuple<int, int>向 TupleElt<int>的转换（自派生类向基类的转换）不是唯一的（有歧义）。
为了打破歧义，需要保证在给定的 Tuple 中每一个 TupleElt 基类都是唯一的。一个方式是将
这个值的“高度”信息（也就是 tail 元组的长度信息）编码进元组中。元组最后一个元素的
高度会被存储生 0，倒数第一个元素的长度会被存储成 1，以此类推：
     */
    
    /*
    template<unsigned Height, typename T>
    class TupleElt {
      T value;
    public:
      TupleElt() = default;
    
      template<typename U>
      TupleElt(U &&other) : value(std::forward<U>(other)) {}
    
      T &get() { return value; }
    
      T const &get() const { return value; }
    };
  
    // 通过这一方式，就能够实现一个即使用了 EBCO 优化，又能保持元素的初始化顺序，并支持包含相同类型元素的元组：
    template<typename... Types>
    class Tuple;
  
    // recursive case:
    template<typename Head, typename... Tail>
    class Tuple<Head, Tail...>
      : private TupleElt<sizeof...(Tail), Head>, private Tuple<Tail...> {
      template<unsigned I, typename... Elements>

      friend auto get(Tuple<Elements...>& t) -> decltype(getHeight<sizeof...(Elements)-I-1>(t));
      using HeadElt = TupleElt<sizeof...(Tail), Head>;
    public:
      Head &getHead() {
        return static_cast<HeadElt *>(this)->get();
      }
      
    
      Head const &getHead() const {
        return static_cast<HeadElt const *>(this)->get();
      }
    
      Tuple<Tail...> &getTail() { return *this; }
    
      Tuple<Tail...> const &getTail() const { return *this; }
    };
  
    // basis case:
    template<>
    class Tuple<> {
      // no storage required
    };
  
    // 基于这一实现，下面的程序：
    struct A {
      A() {
        std::cout << "A()" << '\n';
      }
    };
  
    struct B {
      B() {
        std::cout << "B()" << '\n';
      }
    };
  
    int main() {
      Tuple<A, char, A, char, B> t1;
      std::cout << sizeof(t1) << " bytes" << '\n';
    }
    */

/*
会打印出：
A()
A()
B()
5 bytes
  从中可以看出，EBCO 使得内存占用减少了一个字节（减少的内容是空元组 Tuple<>）。但是
  请注意 A 和 B 都是空的类，这暗示了进一步用 EBCO 进行优化的可能。如果能够安全的从其
  元素类型继承的话，那么就让 TupleElt 继承自其元素类型（这一优化不需要更改 Tuple 的定
  义）：
  */

  
    template<unsigned Height, typename T, bool = std::is_class<T>::value && !std::is_final<T>::value>
    class TupleElt;
  
    template<unsigned Height, typename T>
    class TupleElt<Height, T, false> {
      T value;
    public:
      TupleElt() = default;
    
      template<typename U>
      TupleElt(U &&other) : value(std::forward<U>(other)) {}
    
      T &get() { return value; }
    
      T const &get() const { return value; }
    };
  
    template<unsigned Height, typename T>
    class TupleElt<Height, T, true> : private T {
    public:
      TupleElt() = default;
    
      template<typename U>
      TupleElt(U &&other) : T(std::forward<U>(other)) {}
    
      T &get() { return *this; }
    
      T const &get() const { return *this; }
    };
    
    template<typename... Types>
    class Tuple;
    
    template <typename T, T val>
    struct CTValue {
      static constexpr T value = val;
    };
  
    // recursive case:
    template<typename Head, typename... Tail>
    class Tuple<Head, Tail...>
      : private TupleElt<sizeof...(Tail), Head>, private Tuple<Tail...> {
      template<unsigned I, typename... Elements>
    
      friend auto get(Tuple<Elements...>& t) -> decltype(getHeight<sizeof...(Elements)-I-1>(t));
      
      using HeadElt = TupleElt<sizeof...(Tail), Head>;
    public:
  
      template<typename T, T Index>
      auto& operator[](CTValue<T, Index>) {
        return get<Index>(*this);
      }
      
      Head &getHead() {
        return static_cast<HeadElt *>(this)->get();
      }
    
    
      Head const &getHead() const {
        return static_cast<HeadElt const *>(this)->get();
      }
    
      Tuple<Tail...> &getTail() { return *this; }
    
      Tuple<Tail...> const &getTail() const { return *this; }
    };
    
    
    /*
    当提供给 TupleElt 的模板参数是一个可以被继承的类的时候，它会从该模板参数做 private
    继承，从而也可以将 EBCO 用于被存储的值。有了这些变化，之前的程序会打印出：
    A()
    A()
    B()
    2 bytes
     */
    
    /*
     25.5.2 常数时间的 get()
在使用元组的时候，get()操作的使用是非常常见的，但是其递归的实现方式需要用到线性次
数的模板实例化，这会影响编译所需要的时间。幸运的是，基于在之前章节中介绍的 EBCO，
可以实现一种更高效的 get，我们接下来会对其进行讨论。
主要的思路是，当用一个（基类类型的）参数去适配一个（派生类类型的）参数时，模板参
数推导（参见第 15 章）会为基类推断出模板参数的类型。因此，如果我们能够计算出目标
元素的高度 H，就可以不用遍历所有的索引，也能够基于从 Tuple 的特化结果向 TupleElt<H,T>
（T 的类型由推断得到）的转化提取出相应的元素：
     */

    template<unsigned H, typename T>
    T &getHeight(TupleElt<H, T> &te) {
      return te.get();
    }
  
    template<typename... Types>
    class Tuple;
  
    template<unsigned I, typename... Elements>
    auto get(Tuple<Elements...> &t) -> decltype(getHeight<sizeof...(Elements) - I - 2>(t)) {
      return getHeight<sizeof...(Elements) - I - 2>(t);
    }
  
    /*
    由于 get<I>(t)接收目标元素（从元组头部开始计算）的索引 I 作为参数，而元组的实际存储
      是以高度 H 来衡量的（从元组的末尾开始计算），因此需要用 H 来计算 I。真正的查找工
    作是由调用 getHeight()时的参数推导执行的：由于 H 是在函数调用时显示指定的，因此它的
      值是确定的，这样就只会有一个 TupleElt 会被匹配到，其模板参数 T 则是通过推断得到的。
    这里必须要将 getHeight()声明伟 Tuple 的 friend，否则将无法执行从派生类向 private 父类的
      转换。比如：
      */
    // inside the recursive case for class template Tuple:
    // 由于我们已经将繁杂的索引匹配工作转移到了编译器的模板推断那里，因此这一实现方式只需要常数数量的模板实例化。
    
    /*
     25.6 元组下标
理论上也可以通过定义 operator[]来访问元组中的元素，这和在 std::vector 中定义 operator[]
的情况类似。不过和 std::vector 不同的是，元组中元素的类型可以不同，因此元组的 operator[]
必须是一个模板，其返回类型也需要随着索引的不同而不同。这反过来也就要求每一个索引
都要有不同的类型，因为需要根据索引的类型来决定元素的类型。
使用在第 24.3 节介绍的类模板 CTValue，可以将数值索引编码进一个类型中。将其用于 Tuple
下标运算符定义的代码如下：
     template<typename T, T Index>
auto& operator[](CTValue<T, Index>) {
return get<Index>(*this);
}
然后就可以基于被传递的CTValue类型的参数，用其中的索引信息去执行相关的get<>()调用。
上述代码的用法如下：
auto t = makeTuple(0, '1', 2.2f, std::string{"hello"});
auto a = t[CTValue<unsigned, 2>{}];
auto b = t[CTValue<unsigned, 3>{}];
变量 a 和 b 分别会被 Tuple t 中的第三个和第四个参数初始化成相应的类型和数值。
为了让常量索引的使用变得更方便，我们可以用 constexpr 实现一种字面常量运算符，专门
用来直接从以_c 结尾的常规字面常量，计算出所需的编译期数值字面常量：
     */
  
    // convert single char to corresponding int value at compile time:
    
    constexpr int toInt(char c) {
      // hexadecimal letters:
      if (c >= 'A' && c <= 'F') { return static_cast<int>(c) - static_cast<int>('A') + 10; }
      
      if (c >= 'a' && c <= 'f') { return static_cast<int>(c) - static_cast<int>('a') + 10; }
      
      // other (disable '.' for floating-point literals):
      assert(c >= '0' && c <= '9');
      return static_cast<int>(c) - static_cast<int>('0');
      
    }
    
    // parse array of chars to corresponding int value at compile time:
    template<std::size_t N>
    constexpr int parseInt(char const (&arr)[N]) {
      int base = 10; // to handle base (default: decimal)
      int offset = 0; // to skip prefixes like 0x
      if (N > 2 && arr[0] == '0') {
        switch (arr[1]) {
          case 'x': //prefix 0x or 0X, so hexadecimal
          case 'X':
            base = 16;
            offset = 2;
            break;
            
          case 'b': //prefix 0b or 0B (since C++14), so binary
          case 'B':
            base = 2;
            offset = 2;
            break;
            
          default: //prefix 0, so octal
            base = 8;
            offset = 1;
            break;
        }
      }
      
      // iterate over all digits and compute resulting value:
      int value = 0;
      int multiplier = 1;
      for (std::size_t i = 0; i < N - offset; ++i) {
        
        if (arr[N-1-i] != '\'') { //ignore separating single quotes (e.g. in 1'000)
          value += toInt(arr[N-1-i]) * multiplier;
          multiplier *= base;
        }
        
      }
      return value;
    }
    
// literal operator: parse integral literals with suffix _c as sequence of chars:
    template<char... cs>
    constexpr auto operator"" _c() {
      return CTValue<int, parseInt<sizeof...(cs)>({cs...})>{};
    }
    
    /*
     此处我们用到了这样一个事实，对于数值字面常量，可以用字面常量运算符推导出该字面常
量的每一个字符，并将其用作字面常量运算符模板的参数（参见第 15.5.1 节）。然后将这些
字符传递给一个 constexpr 类型的辅助函数 parseInt()（它可以计算出字符串序列的值，并将
其按照 CTValue 类型返回）。比如：

42_c 生成 CTValue<int,42>

0x815_c 生成 CTValue<int,2069>

0b1111’1111_c 生成 CTValue<int,255>
注意该程序不会处理浮点型字面值常量。对这种情况，相应的 assert 语句会触发编译期错误，
因为这是一个运行期的特性，不能用在编译期上下文中。
基于以上内容，可以像下面这样使用元组：
auto t = makeTuple(0, ’1’, 2.2f, std::string{"hello"});
auto c = t[2_c];
auto d = t[3_c];
这一方式同样被 Boost.Hana 采用，它是一个适用于类型和数值计算的元编程库。
25.7 后记
元组的构造，是诸多由不同程序员独立尝试的模板应用中的一个。Boost.Tuple 库是 C++中最
流行的一种元组的实现方式，并最终发展成 C++11 中的 std::tuple。
在 C++之前，很多元组的实现方式是基于递归的 pair 结构；在本书的第一版中通过其
“recursive duos”展示了这样一种实现方式。另一种有趣的实现方式是由 Andrei Alexandrescu
在[AlexandrescuDesign]中开发的。他将元组中的类型列表和数据列表明确地分离开，并将
typelist（第 24 章）的概念用作元组的实现基础。
在 C++11 的实现中则使用了变参模板，这样可以通过参数包明确的为元组捕获类型列表，从
而 不 需 要 使 用 递 归 的
pair 。 包 展 开 以 及 索 引 列 表 概 念 的 引 入
[GregorJarviPowellVariadicTemplates]，使得递归模板实例化变得简单且高效，从而也使元组
变得更实用。 索引列表对元组和类型列表的算法是如此的关键，以至于编译器为之包含了
一个内置的模板别名__make_integer_seq<S,T,N>，它会在不需要额外的模板实例化的情况下
展开成 S<T,0,1,..., N>，从而促进了 std::make_index_sequence 和 make_integer_sequence 的应
用。
五车书馆
366
元组是最广泛使用的异质容器，但是并不是唯一一个。Boost.Fusion 提供了其它一些与常规
容器对应的异质容器，比如异质的 list，deque，set 和 map。最重要的是，它还提供了一个
与 C++标准库使用了相同的抽象和术语（比如 iterators，sequences 和 containers）的框架，
使用该框架可以为异质集合编写算法。
Boost.Hana 采用了一些 Boost.MPL 和 Boost.Fusion 中的理念，它们都在 C++11 迈向成熟之前
就已被设计和实现，并在之后被用 C++11（以及 C++14）的新特性重新实现。最终就产生出
了一个简洁的、提供了强大的异质计算能力的库。
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
main()
//main_ch25_5()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch25_5::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch25_5::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch25_5::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch25_5::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch25_5::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}
