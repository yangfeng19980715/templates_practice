//
// Created by yangfeng on 9/12/2022.
//

#include "../headers.h"

using namespace std;

/*
24.3 非类型类型列表（Nontype Typelists）
通过类型列表，有非常多的算法和操作可以用来描述并操作一串类型。某些情况下，还会希
望能够操作一串编译期数值，比如多维数组的边界，或者指向另一个类型列表中的索引。
有很多种方法可以用来生成一个包含编译期数值的类型列表。一个简单的办法是定义一个类
模板 CTValue（compile time value），然后用它表示类型列表中某种类型的值：
template<typename T, T Value>
struct CTValue
{
static constexpr T value = Value;
};
用它就可以生成一个包含了最前面几个素数的类型列表：
using Primes = Typelist<CTValue<int, 2>, CTValue<int, 3>,
CTValue<int, 5>, CTValue<int, 7>,
CTValue<int, 11>>;
这样就可以对类型列表中的数值进行数值计算，比如计算这些素数的乘积。
五车书馆
335
首先 MultiPlyT 模板接受两个类型相同的编译期数值作为参数，并生成一个新的、类型相同
的编译期数值：
template<typename T, typename U>
struct MultiplyT;
template<typename T, T Value1, T Value2>
struct MultiplyT<CTValue<T, Value1>, CTValue<T, Value2>> {
public:
using Type = CTValue<T, Value1 * Value2>;
};
template<typename T, typename U>
using Multiply = typename MultiplyT<T, U>::Type;
然后结合 MultiplyT，下面的表达式就会返回所有 Primes 中素数的乘积：
Accumulate<Primes, MultiplyT, CTValue<int, 1>>::value
不过这一使用 Typelist 和 CTValue 的方式过于复杂，尤其是当所有数值的类型相同的时候。
可以通过引入 CTTypelist 模板别名来进行优化，它提供了一组包含在 Typelist 中、类型相同
的数值：
template<typename T, T… Values>
using CTTypelist = Typelist<CTValue<T, Values>…>;
这样就可以使用 CTTypelist 来定义一版更为简单的 Primes（素数）：
using Primes = CTTypelist<int, 2, 3, 5, 7, 11>;
这一方式的唯一缺点是，别名终归只是别名，当遇到错误的时候，错误信息可能会一直打印
到 CTValueTypes 中的底层 Typelist，导致错误信息过于冗长。为了解决这一问题，可以定义
一个能够直接存储数值的、全新的类型列表类 Valuelist：
template<typename T, T… Values>
struct Valuelist {
};
template<typename T, T… Values>
struct IsEmpty<Valuelist<T, Values…>> {
static constexpr bool value = sizeof…(Values) == 0;
};
template<typename T, T Head, T… Tail>
struct FrontT<Valuelist<T, Head, Tail…>> {
using Type = CTValue<T, Head>;
static constexpr T value = Head;
};
五车书馆
336
template<typename T, T Head, T… Tail>
struct PopFrontT<Valuelist<T, Head, Tail…>> {
using Type = Valuelist<T, Tail…>;
};
template<typename T, T… Values, T New>
struct PushFrontT<Valuelist<T, Values…>, CTValue<T, New>> {
using Type = Valuelist<T, New, Values…>;
};
template<typename T, T… Values, T New>
struct PushBackT<Valuelist<T, Values…>, CTValue<T, New>> {
using Type = Valuelist<T, Values…, New>;
};
通过代码中提供的 IsEmpty，FrontT，PopFrontT 和 PushFrontT，Valuelist 就可以被用于本章
中介绍的各种算法了。PushBackT 被实现为一种算法的特例化，这样做可以降低编译期间该
操作的计算成本。比如 Valuelist 可以被用于前面定义的算法 InsertionSort：
template<typename T, typename U>
struct GreaterThanT;
template<typename T, T First, T Second>
struct GreaterThanT<CTValue<T, First>, CTValue<T, Second>> {
static constexpr bool value = First > Second;
};
void valuelisttest()
{
using Integers = Valuelist<int, 6, 2, 4, 9, 5, 2, 1, 7>;
using SortedIntegers = InsertionSort<Integers, GreaterThanT>;
static_assert(std::is_same_v<SortedIntegers, Valuelist<int, 9, 7,
6, 5, 4, 2, 2, 1>>, "insertion sort failed");
}
注意在这里可以提供一种用字面值常量来初始化 CTValue 的功能，比如：
auto a = 42_c; // initializes a as CTValue<int,42>
相关细节请参见 25.6 节。
24.3.1 可推断的非类型参数
在 C++17 中，可以通过使用一个可推断的非类型参数（结合 auto）来进一步优化 CTValue 的
实现：
五车书馆
337
template<auto Value>
struct CTValue
{
static constexpr auto value = Value;
};
这样在使用 CTValue 的时候就可以不用每次都去指定一个类型了，从而简化了使用方式：
using Primes = Typelist<CTValue<2>, CTValue<3>, CTValue<5>, CTValue<7>,
CTValue<11>>;
在 C++17 中也可以对 Valuelist 执行同样的操作，但是结果可能不一定会变得更好。正如在第
15.10.1 节提到的那样，对一个非类型参数包进行类型推断时，各个参数可以不同：
template<auto… Values>
class Valuelist { };
int x;
using MyValueList = Valuelist<1,’a’, true, &x>;
虽然这样一个列表可能也很有用，但是它和之前要求元素类型必须相同的 Valuelist 已经不一
样了。虽然我们也可以要求其所有元素的类型必须相同（参见 15.10.1 节的讨论），但是对
于一个空的 Valuelist<>而言，其元素类型却是未知的。
 */

namespace ch24_3 {

  namespace case1 {
  
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
//main_ch24_3()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch24_3::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch24_3::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch24_3::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch24_3::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch24_3::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}