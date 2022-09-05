//
// Created by yangfeng on 2022/9/5.
//

#include "../headers.h"

using namespace std;

/*
第 23 章 元编程
元编程的意思是“编写一个程序”。也就是说，我们构建了可以被编程系统用来产生新代码
的代码，而且新产生的代码实现了我们真正想要的功能。通常名词“元编程”暗示了一种自
反的属性：元编程组件是其将要为之产生一部分代码的程序的一部分（比如，程序中一些附
加的或者不同的部分）。
为什么需要元编程？和其它编程技术一样，目的是用尽可能少的“付出”，换取尽可能多的
功能，其中“付出”可以用代码长度、维护成本之类的事情来衡量。元编程的特性之一是在
编译期间（at translation time，翻译是否准确？）就可以进行一部分用户定义的计算。其动
机通常是性能（在 translation time 执行的计算通常可以被优化掉）或者简化接口（元-程序
通常要比其展开后的结果短小一些），或者两者兼而有之。
元编程通常依赖于萃取和类型函数等概念，详见第 19 章。我们建议在继续接下来的内容之
前，最好先熟悉下第 19 章的内容。
23.1 现代 C++元编程的现状
C++元编程是随着时间发展逐渐成形的。我们先来分类讨论多种在现代 C++中经常使用的元
编程方法。
2.3.1.1 值元编程（Value Metaprogramming）
在本书第一版中，我们局限于原始 C++标准的特性（发布于 1988 年，在 2003 年做了修订）。
在当时，构建简单的编译期（“meta-”）计算程序也会是一个小的挑战。因此我们曾在本
章中花了很大篇幅来做这些事情；一个非常复杂的例子是在编译期间用递归的模板实例化来
计算一个整数的平方根。不过正如 8.2 节介绍的那样，在 C++11，尤其是 C++14 中通过使用
constexpr 函数，可以大大降低这一挑战的难度。比如在 C++14 中，一个在编译期计算平方
根的函数可以被简单的写成这样：
template<typename T>
constexpr T sqrt(T x)
{
// handle cases where x and its square root are equal as a special
case to simplify
// the iteration criterion for larger x:
if (x <= 1) {
return x;
}
五车书馆
304
// repeatedly determine in which half of a [lo, hi] interval the square
root of x is located,
// until the interval is reduced to just one value:
T lo = 0, hi = x;
for (;;) {
auto mid = (hi+lo)/2, midSquared = mid*mid;
if (lo+1 >= hi || midSquared == x) {
// mid must be the square root:
return mid;
}
//continue with the higher/lower half-interval:
if (midSquared < x) {
lo = mid;
} else {
hi = mid;
}
}
}
该算法通过反复地选取一个包含 x 的平方根的中间值来计算结果（为了让收敛标准比较简
单，对 0 和 1 的平方根做了特殊处理）。该 sqrt()函数可以被在编译期或者运行期间计算：
static_assert(sqrt(25) == 5, ""); //OK (evaluated at compile time)
static_assert(sqrt(40) == 6, ""); //OK (evaluated at compile time)
std::array<int, sqrt(40)+1> arr; //declares array of 7 elements (compile
time)
long long l = 53478;
std::cout << sqrt(l) << '\n'; //prints 231 (evaluated at run time)
在运行期间这一实现方式可能不是最高效的（在这里去开发机器的各种特性通常是值得的），
但是由于该函数意在被用于编译期计算，绝对的效率并没有可移植性重要。注意在这个例子
中并没有什么“模板魔法”，只是用到了常规的模板参数推断。相关代码是“纯 C++”的，
并没有特别难以理解的地方。
上面介绍的值元编程（比如在编译期间计算某些数值）偶尔会非常有用，但是在现代 C++中
还有另外两种可用的元编程方式（在 C++14 和 C++17 中）：类型元编程和混合元编程。
23.1.2 类型元编程
在第 19 章中讨论某些萃取模板的时候已经遇到过一种类型元编程，它接受一个类型作为输
入并输出一个新的类型。比如 RemoveReferenceT 类模板会计算引用类型所引用对象的真正
类型。但是在第 19 章中实现的例子只会计算很初级的类型操作。通过递归的模板实例化--
这也是主要的基于模板的元编程手段--我们可以实现更复杂的类型计算。
五车书馆
305
考虑如下例子：
// primary template: in general we yield the given type:
template<typename T>
struct RemoveAllExtentsT {
using Type = T;
};
// partial specializations for array types (with and without bounds):
template<typename T, std::size_t SZ>
struct RemoveAllExtentsT<T[SZ]> {
using Type = typename RemoveAllExtentsT<T>::Type;
};
template<typename T>
struct RemoveAllExtentsT<T[]> {
using Type = typename RemoveAllExtentsT<T>::Type;
};
template<typename T>
using RemoveAllExtents = typename RemoveAllExtentsT<T>::Type;
这里 RemoveAllExtents 就是一种类型元函数（比如一个返回类型的计算设备），它会从一个
类型中移除掉任意数量的顶层“数组层”。就像下面这样：
RemoveAllExtents<int[]> // yields int
RemoveAllExtents<int[5][10]> // yields int
RemoveAllExtents<int[][10]> // yields int
RemoveAllExtents<int(*)[5]> // yields int(*)[5]
元函数通过偏特化来匹配高层次的数组，递归地调用自己并最终完成任务。
如果数值计算的功能只适用于标量，那么其应用会很受限制。幸运的是，几乎有所得语言都
至少有一种数值容器，这可以大大的提高该语言的能力（而且很多语言都有各种各样的容器，
比如 array/vector，hash table 等）。对于元编程也是这样：增加一个“类型容器”会大大的
提高其自身的适用范围。幸运的是，现代 C++提供了可以用来开发类似容器的机制。第 24
章开发的 Typelist<...>类模板，就是这一类型的类型容器。
23.1.3 混合元编程
通过使用数值元编程和类型元编程，可以在编译期间计算数值和类型。但是最终我们关心的
还是在运行期间的效果，因此在运行期间的代码中，我们将元程序用在那些需要类型和常量
的地方。不过元编程能做的不仅仅是这些：我们可以在编译期间，以编程的方式组合一些有
运行期效果的代码。我们称之为混合元编程。
下面通过一个简单的例子来说明这一原理：计算两个 std::array 的点乘结果。回忆一下，sta：：
array 是具有固定长度的容器模板，其声明如下：
namespace std {
五车书馆
306
template<typename T, size_t N> struct array;
}
其中 N 是 std::array 的长度。假设有两个类型相同的 sta::array 对象，其点乘结果可以通过如
下方式计算：
template<typename T, std::size_t N>
auto dotProduct(std::array<T, N> const& x, std::array<T, N>
const& y)
{
T result{};
for (std::size_t k = 0; k<N; ++k) {
result += x[k]*y[k];
}
return result;
}
如果对 for 循环进行直接编译的话，那么就会生成分支指令，相比于直接运行如下命令，这
在一些机器上可能会增加运行成本：
result += x[0]*y[0];
result += x[1]*y[1];
result += x[2]*y[2];
result += x[3]*y[3];
…
幸运的是，现代编译器会针对不同的平台做出相应的最为高效的优化。但是为了便于讨论，
下面重新实现一版不需要 loop 的 dotProduct():
template<typename T, std::size_t N>
struct DotProductT {
static inline T result(T* a, T* b)
{
return *a * *b + DotProduct<T, N-1>::result(a+1,b+1);
}
};
// partial specialization as end criteria
template<typename T>
struct DotProductT<T, 0> {
static inline T result(T*, T*) {
return T{};
}
};
template<typename T, std::size_t N>
auto dotProduct(std::array<T, N> const& x,
std::array<T, N> const& y)
{
五车书馆
307
return DotProductT<T, N>::result(x.begin(), y.begin());
}
新的实现将计算放在了类模板 DotProductT 中。这样做的目的是为了使用类模板的递归实例
化来计算结果，并能够通过部分特例化来终止递归。注意例子中 DotProductT 的每一次实例
化是如何计算点乘中的一项结果、以及所有剩余结果的。对于 std::arrat<T,N>，会对主模板
进行 N 次实例化，对部分特例化的模板进行一次实例化。为了保证效率，编译期需要将每
一次对静态成员函数 result()的调用内联（inline）。幸运的是，即使使用的时中等优化选项，
编译器也会这样做。
这段代码的主要特点是它融合了编译期计算（这里通过递归的模板实例化实现，这决定了代
码的整体结构）和运行时计算（通过调用 result()，决定了具体的运行期间的效果）。
我们之前提到过，“类型容器”可以大大提高元编程的能力。我们同样看到固定长度的 array
在混合元编程中也非常有用。但是混合元编程中真正的“英雄容器”是 tuple（元组）。Tuple
是一串数值，且其中每个值的类型可以分别指定。C++标准库中包含了支持这一概念的类模
板 std::tuple。比如：
std::tuple<int, std::string, bool> tVal{42, "Answer", true};
定义的变量 tVal 包含了三个类型分别为 int, std::string 和 bool 的值。因为 tuple 这一类容器
在现代 C++编程中非常重要，我们将在第 25 章对其进行更深入的讨论。tVal 的类型和下面
这个简单的 struct 类型非常类似：
struct MyTriple {
int v1;
std::string v2;
bool v3;
};
既然对于 array 类型和（简单）的 struct 类型，我们有比较灵活的 std::array 和 std::tuple 与
之对应，那么你可能会问，与简单的 union 对应的类似类型是否对混合元编程也很有益。答
案是“yes”。C++标准库在 C++17 中为了这一目的引入了 std::variant 模板，在第 26 章中我
们会介绍一个类似的组件。
由于 std::tuple 和 std::variant 都是异质类型（与 struct 类似），使用这些类型的混合元编程
有时也被称为“异质元编程”。
23.1.4 将混合元编程用于“单位类型”（Units Types，可能
翻译的不恰当）
另一个可以展现混合元编程威力的例子是那些实现了不同单位类型的数值之间计算的库。相
应的数值计算发生在程序运行期间，而单位计算则发生在编译期间。
五车书馆
308
下面会以一个极度精简的例子来做讲解。我们将用一个基于主单位的分数来记录相关单位。
比如如果时间的主单位是秒，那么就用 1/1000 表示 1 微秒，用 60/1 表示一分钟。因此关键
点就是要定义一个比例类型，使得每一个数值都有其自己的类型：
template<unsigned N, unsigned D = 1>
struct Ratio {
static constexpr unsigned num = N; // numerator
static constexpr unsigned den = D; // denominator
using Type = Ratio<num, den>;
};
现在就可以定义在编译期对两个单位进行求和之类的计算：
// implementation of adding two ratios:
template<typename R1, typename R2>
struct RatioAddImpl
{
private:
static constexpr unsigned den = R1::den * R2::den;
static constexpr unsigned num = R1::num * R2::den + R2::num *
R1::den;
public:
typedef Ratio<num, den> Type;
};
// using declaration for convenient usage:
template<typename R1, typename R2>
using RatioAdd = typename RatioAddImpl<R1, R2>::Type;
这样就可以在编译期计算两个比率之和了：
using R1 = Ratio<1,1000>;
using R2 = Ratio<2,3>;
using RS = RatioAdd<R1,R2>; //RS has type Ratio<2003,2000>
std::cout << RS::num << '/' << RS::den << '\n'; //prints 2003/3000
using RA = RatioAdd<Ratio<2,3>,Ratio<5,7>>; //RA has type
Ratio<29,21>
std::cout << RA::num << '/' << RA::den << '\n'; //prints 29/21
然后就可以为时间段定义一个类模板，用一个任意数值类型和一个 Ratio<>实例化之后的类
型作为其模板参数：
// duration type for values of type T with unit type U:
template<typename T, typename U = Ratio<1>>
class Duration {public:
using ValueType = T;
using UnitType = typename U::Type;
private:
ValueType val;
五车书馆
309
public:
constexpr Duration(ValueType v = 0)
: val(v) {
}
constexpr ValueType value() const {
return val;
}
};
比较有意思的地方是对两个 Durations 求和的 operator+运算符的定义：
// adding two durations where unit type might differ:
template<typename T1, typename U1, typename T2, typename U2>
auto constexpr operator+(Duration<T1, U1> const& lhs,
Duration<T2, U2> const& rhs)
{
// resulting type is a unit with 1 a nominator and
// the resulting denominator of adding both unit type fractions
using VT = Ratio<1,RatioAdd<U1,U2>::den>;
// resulting value is the sum of both values
// converted to the resulting unit type:
auto val = lhs.value() * VT::den / U1::den * U1::num +
rhs.value() * VT::den / U2::den * U2::num;
return Duration<decltype(val), VT>(val);
}
这里参数所属的单位类型可以不同，比如分别为 U1 和 U2。然后可以基于 U1 和 U2 计算最
终的时间段，其类型为一个新的分子为 1 的单位类型。基于此，可以编译如下代码：
int x = 42;
int y = 77;
auto a = Duration<int, Ratio<1,1000>>(x); // x milliseconds
auto b = Duration<int, Ratio<2,3>>(y); // y 2/3 seconds
auto c = a + b; //computes resulting unit type 1/3000 seconds//and
generates run-time code for c = a*3 + b*2000
此处“混合”的效果体现在，在计算 c 的时候，编译器会在编译期决定结果的单位类型
Ratio<1,3000>，并产生出可以在程序运行期间计算最终结果的代码（结果会被根据单位类型
进行调整）。
由于数值类型是由模板参数决定的，因此可以将 int 甚至是异质类型用于 Duration 类：
auto d = Duration<double, Ratio<1,3>>(7.5); // 7.5 1/3 seconds
auto e = Duration<int, Ratio<1>>(4); // 4 seconds
auto f = d + e; //computes resulting unit type 1/3 seconds
// and generates code for f = d + e*3
五车书馆
310
而且如果相应的数值在编译期是已知的话，编译器甚至可以在编译期进行以上计算（因为上
文中的 operator+是 constexpr）。
C++中的 std::chrono 类模板使用了类似于上文中的内容，但是做了一些优化，比如使用已定
义的单位（比如 std::chrono::miliseconds），支持时间段常量（比如 10ms），以及能够处理
溢出。
 */

namespace ch23_1 {
  
  // 2.3.1.1 值元编程（Value Metaprogramming）
  namespace case1 {
    
    template<typename T>
    constexpr T sqrt(T x)   // in c++ 14
    {
      // handle cases where x and its square root are equal as a special case to simplify
      // the iteration criterion for larger x:
      if (x <= 1)
        return x;
      // repeatedly determine in which half of a [lo, hi] interval the square root of x is located,
      // until the interval is reduced to just one value:
      T lo = 0, hi = x;
      for (;;) {
        auto mid = (hi + lo) / 2, midSquared = mid * mid;
        if (lo + 1 >= hi || midSquared == x)
          return mid;   // mid must be the square root:
        
        //continue with the higher/lower half-interval:
        if (midSquared < x)
          lo = mid;
        else
          hi = mid;
      }
      
    }
    
    void test() {
      static_assert(case1::sqrt(25) == 5, "error of case1::sqrt");
      cout << "hello, world" << endl;
    }
    
  }
  
  /*
  23.1.2 类型元编程
  在第 19 章中讨论某些萃取模板的时候已经遇到过一种类型元编程，它接受一个类型作为输
入并输出一个新的类型。比如 RemoveReferenceT 类模板会计算引用类型所引用对象的真正
类型。但是在第 19 章中实现的例子只会计算很初级的类型操作。通过递归的模板实例化--
这也是主要的基于模板的元编程手段--我们可以实现更复杂的类型计算。
   */
  namespace case2 {
    
    // primary template: in general we yield the given type:
    template<typename T>
    struct RemoveAllExtentsT {
      using Type = T;
    };
    
    // partial specializations for array types (with and without bounds):
    template<typename T, std::size_t SZ>
    struct RemoveAllExtentsT<T[SZ]> {
      using Type = typename RemoveAllExtentsT<T>::Type;
    };
    
    template<typename T>
    struct RemoveAllExtentsT<T[]> {
      using Type = typename RemoveAllExtentsT<T>::Type;
    };
    
    template<typename T>
    using RemoveAllExtents = typename RemoveAllExtentsT<T>::Type;
    
    void test() {
      cout << std::is_same_v<RemoveAllExtents<int[3][3]>, int> << endl;
      cout << std::is_same_v<RemoveAllExtents<int[]>, int> << endl;
      cout << std::is_same_v<RemoveAllExtents<int>, int> << endl;
      cout << std::is_same_v<RemoveAllExtents<int (*)[]>, int> << endl;
    }
    
  }
  /*
   23.1.3 混合元编程
   通过使用数值元编程和类型元编程，可以在编译期间计算数值和类型。但是最终我们关心的
还是在运行期间的效果，因此在运行期间的代码中，我们将元程序用在那些需要类型和常量
的地方。不过元编程能做的不仅仅是这些：我们可以在编译期间，以编程的方式组合一些有
运行期效果的代码。我们称之为混合元编程。
   下面通过一个简单的例子来说明这一原理：计算两个 std::array 的点乘结果。
   */
  
  namespace case3 {
    /* 其中 N 是 std::array 的长度。假设有两个类型相同的 sta::array 对象，其点乘结果可以通过如下方式计算： */
    
    template<typename T, std::size_t N>
    auto dotProduct_old(std::array<T, N> const &x, std::array<T, N> const &y) {
      T result{};
      
      for (std::size_t k = 0; k < N; ++k)
        result += x[k] * y[k];
      
      return result;
    }
    /*
    如果对 for 循环进行直接编译的话，那么就会生成分支指令，相比于直接运行如下命令，这
在一些机器上可能会增加运行成本：
result += x[0]*y[0];
result += x[1]*y[1];
result += x[2]*y[2];
result += x[3]*y[3];
…
幸运的是，现代编译器会针对不同的平台做出相应的最为高效的优化。但是为了便于讨论，
下面重新实现一版不需要 loop 的 dotProduct():
     */
    template<typename T, std::size_t N>
    struct DotProductT {
      static inline T result(T* a, T* b)
      {
        return *a * *b + DotProductT<T, N-1>::result(a+1,b+1);
      }
    };
    
    // partial specialization as end criteria
    // 因为函数模板不支持偏特化，所以这类代码都是这样的写法
    template<typename T>
    struct DotProductT<T, 0> {
      
      static inline T result(T*, T*) {
        return T{};
      }
      
    };
    
    template<typename T, std::size_t N>
    auto dotProduct(std::array<T, N> const& x, std::array<T, N> const& y)
    {
      return DotProductT<T, N>::result(x.begin(), y.begin());
    }
    
    void test() {
    
    }
    
  }
  
  /*
   23.1.4 将混合元编程用于“单位类型”（Units Types，可能
翻译的不恰当）
另一个可以展现混合元编程威力的例子是那些实现了不同单位类型的数值之间计算的库。相
应的数值计算发生在程序运行期间，而单位计算则发生在编译期间。
   */
  
  namespace case4 {
    /*
    下面会以一个极度精简的例子来做讲解。我们将用一个基于主单位的分数来记录相关单位。
比如如果时间的主单位是秒，那么就用 1/1000 表示 1 微秒，用 60/1 表示一分钟。因此关键
点就是要定义一个比例类型，使得每一个数值都有其自己的类型：
     */
    template<unsigned N, unsigned D = 1>
    struct Ratio {
      static constexpr unsigned num = N; // numerator
      static constexpr unsigned den = D; // denominator
      using Type = Ratio<num, den>;
    };
  
    // 现在就可以定义在编译期对两个单位进行求和之类的计算：
    // implementation of adding two ratios:
    template<typename R1, typename R2>
    struct RatioAddImpl
    {
    private:
      static constexpr unsigned den = R1::den * R2::den;
      static constexpr unsigned num = R1::num * R2::den + R2::num *
                                                          R1::den;
    public:
      typedef Ratio<num, den> Type;
    };
    // using declaration for convenient usage:
    template<typename R1, typename R2>
    using RatioAdd = typename RatioAddImpl<R1, R2>::Type;
  
    // 然后就可以为时间段定义一个类模板，用一个任意数值类型和一个 Ratio<>实例化之后的类型作为其模板参数：
    // duration type for values of type T with unit type U:
    template<typename T, typename U = Ratio<1>>
    class Duration {public:
      using ValueType = T;
      using UnitType = typename U::Type;
    private:
      ValueType val;
    public:
      constexpr Duration(ValueType v = 0)
        : val(v) {
      }
      constexpr ValueType value() const {
        return val;
      }
    };
    // 比较有意思的地方是对两个 Durations 求和的 operator+运算符的定义：
    // adding two durations where unit type might differ:
    template<typename T1, typename U1, typename T2, typename U2>
    auto constexpr operator+(Duration<T1, U1> const& lhs,
                             Duration<T2, U2> const& rhs)
    {
      // resulting type is a unit with 1 a nominator and
      // the resulting denominator of adding both unit type fractions
      using VT = Ratio<1,RatioAdd<U1,U2>::den>;
      // resulting value is the sum of both values
      // converted to the resulting unit type:
      auto val = lhs.value() * VT::den / U1::den * U1::num +
                 rhs.value() * VT::den / U2::den * U2::num;
      return Duration<decltype(val), VT>(val);
    }
    /*
    这里参数所属的单位类型可以不同，比如分别为 U1 和 U2。然后可以基于 U1 和 U2 计算最
      终的时间段，其类型为一个新的分子为 1 的单位类型。基于此，可以编译如下代码：
      */
    int x = 42;
    int y = 77;
    auto a = Duration<int, Ratio<1,1000>>(x); // x milliseconds
    auto b = Duration<int, Ratio<2,3>>(y); // y 2/3 seconds
    auto c = a + b; //computes resulting unit type 1/3000 seconds//and generates run-time code for c = a*3 + b*2000
      /*
    此处“混合”的效果体现在，在计算 c 的时候，编译器会在编译期决定结果的单位类型
      Ratio<1,3000>，并产生出可以在程序运行期间计算最终结果的代码（结果会被根据单位类型
      进行调整）。
    由于数值类型是由模板参数决定的，因此可以将 int 甚至是异质类型用于 Duration 类：
       */
    auto d = Duration<double, Ratio<1,3>>(7.5); // 7.5 1/3 seconds
    auto e = Duration<int, Ratio<1>>(4); // 4 seconds
    auto f = d + e; //computes resulting unit type 1/3 seconds
    
    // and generates code for f = d + e*3
    
    /*
    而且如果相应的数值在编译期是已知的话，编译器甚至可以在编译期进行以上计算（因为上
      文中的 operator+是 constexpr）。
    C++中的 std::chrono 类模板使用了类似于上文中的内容，但是做了一些优化，比如使用已定
      义的单位（比如 std::chrono::miliseconds），支持时间段常量（比如 10ms），以及能够处理
      溢出。
      */
    
    void test() {
      // 这样就可以在编译期计算两个比率之和了：
      using R1 = Ratio<1,1000>;
      using R2 = Ratio<2,3>;
      using RS = RatioAdd<R1,R2>; //RS has type Ratio<2003,2000>
      ::std::cout << RS::num << '/' << RS::den << '\n'; //prints 2003/3000
      using RA = RatioAdd<Ratio<2,3>,Ratio<5,7>>; //RA has type Ratio<29,21>
      std::cout << RA::num << '/' << RA::den << '\n'; //prints 29/21
    
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
//main_ch23_1()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch23_1::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch23_1::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch23_1::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch23_1::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch23_1::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;
  
  return 0;
}