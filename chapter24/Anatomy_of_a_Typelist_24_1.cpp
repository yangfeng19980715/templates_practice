//
// Created by yangfeng on 9/8/2022.
//

#include "../headers.h"

using namespace std;

/*
 第 24 章 类型列表（Typelists）
高效的编程通常需要用到各种各样的数据结构，元编程也不例外。对于类型元编程，核心的
数据结构是 typelist，和其名字的意思一样，它指的是一个包含了类型的列表。模板元编程
可以操作 typelist 并最终生成可执行程序的一部分。接下来会在本章中讨论使用 typelist 的技
术。由于大多数 typelist 相关的操作都用到了模板元编程，这里假设你已经对模板元编程比
较熟悉（参见第 23 章）。
24.1 类型列表剖析（Anatomy of a Typelist）
类型列表指的是一种代表了一组类型，并且可以被模板元编程操作的类型。它提供了典型的
列表操作方法：遍历列表中的元素，添加元素或者删除元素。但是类型列表和大多数运行期
间的数据结构都不同（比如 std::list），它的值不允许被修改。向类型列表中添加一个元素
并不会修改原始的类型列表，只是会创建一个新的、包含了原始类型列表和新添加元素的类
型列表。对函数式编程语言（比如 Scheme，ML 以及 Haskell）比较熟悉的读者应该会意识
到 C++中的类型列表和这些函数式编程语言中的列表之间的相似性。
类型列表通常是按照类模板特例的形式实现的，它将自身的内容（包含在模板参数中的类型
以及类型之间的顺序）编码到了参数包中。一种将其内容编码到参数包中的类型列表的直接
实现方式如下：
template<typename… Elements>
class Typelist
{};
Typelist 中的元素被直接写成其模板参数。一个空的类型列表被写为 Typelist<>，一个只
包含 int 的类型列表被写为 Typelist<int>。下面是一个包含了所有有符号整型的类型列
表：
using SignedIntegralTypes =
Typelist<signed char, short, int, long, long long>;
操作这个类型列表需要将其拆分，通常的做法是将第一个元素（the head）从剩余的元素中
分离（the tail）。比如 Front 元函数会从类型列表中提取第一个元素：
template<typename List>
class FrontT;
template<typename Head, typename… Tail>
class FrontT<Typelist<Head, Tail…>>
{
public:
using Type = Head;
五车书馆
321
};
template<typename List>
using Front = typename FrontT<List>::Type;
这
样
FrontT<SignedIntegralTypes>::Type
（
或
者
更
简
洁
的
记
作
FrontT<SignedIntegralTypes>）返回的就是 signed char。同样 PopFront 元函数会
删除类型列表中的第一个元素。在实现上它会将类型列表中的元素分为头（head）和尾（tail）
两部分，然后用尾部的元素创建一个新的 Typelist 特例。
template<typename List>
class PopFrontT;
template<typename Head, typename… Tail>
class PopFrontT<Typelist<Head, Tail…>> {
public:
using Type = Typelist<Tail…>;
};
template<typename List>
using PopFront = typename PopFrontT<List>::Type;
PopFront<SignedIntegralTypes>会产生如下类型列表：
Typelist<short, int, long, long long>
同样也可以向类型列表中添加元素，只需要将所有已经存在的元素捕获到一个参数包中，然
后在创建一个包含了所有元素的 TypeList 特例就行：
template<typename List, typename NewElement>
class PushFrontT;
template<typename… Elements, typename NewElement>
class PushFrontT<Typelist<Elements…>, NewElement> {
public:
using Type = Typelist<NewElement, Elements…>;
};
template<typename List, typename NewElement>
using PushFront = typename PushFrontT<List, NewElement>::Type;
和预期的一样，
PushFront<SignedIntegralTypes, bool>
会生成：
Typelist<bool, signed char, short, int, long, long long>
 */

namespace ch24_1 {

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
//main_ch24_1()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch24_1::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch24_1::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch24_1::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch24_1::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch24_1::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}