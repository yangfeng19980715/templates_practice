//
// Created by yangfeng on 9/17/2022.
//

#include "../headers.h"

using namespace std;

/*
 24.4 对包扩展相关算法的优化（Optimizing Algorithms
with Pack Expansions ）
通过使用包展开（参见 12.4.1 节），可以将类型列表迭代的任务转移给编译器。在第 24.2.5
节开发的 Transform 就是一个天生的、适用于包展开的算法，因为它会对所有列表中的元素
执行相同的操作。这样就能够用一种偏特例化的算法对一个类型列表进行转换：
template<typename... Elements, template<typename T> class MetaFun>
class TransformT<Typelist<Elements...>, MetaFun, false>
{
public:
using Type = Typelist<typename MetaFun<Elements>::Type...>;
};
这一实现方式用以一个参数包 Elements 捕获了类型列表中的所有元素。接着它通过将
typename MetaFun<Elements>::Type 用于包展开，将元函数作用于 Elements 中的各个
元素，并生成一个新的类型列表。可以认为这一实现方式更简单一些，因为它不需要递归，
而是非常直观地使用了一些语言特性。除此之外，由于只需要实例化一个 Transform 模板的
实例，它需要的模板实例的数目也更少。不过这个算法需要的 MetaFun 实例的数量依然是
和列表长度成正比的，因为这些实例是该算法的基础，不可能被省略。
其它算法也可以从包展开中获益。比如在第 24.2.4 节介绍的 Reverse 算法，其需要对 PushBack
五车书馆
338
实例化的次数和列表的长度成正比。如果使用了在第 24.2.3 节介绍的、用到了包展开的
PushBack 的话（只需要一个 PushBack 实例），那么 Reverse 的复杂度和最终的列表长度也
成正比。而如果是使用同样在这一节介绍的使用了常规递归方法的 PushBack，由于 PushBack
本身的复杂度和列表的长度也是正比关系，这样就会导致最终的 Reverse 和列表的长度成平
方关系。
也可以基于索引值从一个已有列表中选择一些元素，并生成新的列表。Select 元函数接受一
个类型列表和一个存储索引值的 Valuelist 作为参数，并最终生成一个包含了被索引元素的新
的类型列表：
template<typename Types, typename Indices>
class SelectT;
template<typename Types, unsigned... Indices>
class SelectT<Types, Valuelist<unsigned, Indices...>>
{
public:
using Type = Typelist<NthElement<Types, Indices>...>;
};
template<typename Types, typename Indices>
using Select = typename SelectT<Types, Indices>::Type;
索引值被捕获进参数包 Indices 中，它被扩展成一串指向已有类型列表的 NthElement 类型，
并生成一个新的类型列表。下面的代码展示了一种通过 Select 反转类型列表的方法：
using SignedIntegralTypes = Typelist<signed char, short, int, long, long
long>;
using ReversedSignedIntegralTypes = Select<SignedIntegralTypes,
Valuelist<unsigned, 4, 3, 2, 1, 0>>;
// produces Typelist<long long, long, int, short, signed char>
一个包含了指向另一个列表的索引的非类型类型列表，通常被称为索引列表（index list，或
者索引序列，index sequence），可以通过它来简化甚至省略掉递归计算。在第 25.3.4 节会
对索引列表进行详细介绍。
 */

namespace ch24_4 {
  
  /*
    
   */
  
  namespace case1 {
    template <typename ...T>
    struct Typelist { };
    
    template <typename Elements>
    struct IsEmpty {
      static constexpr bool value = false;
    };
    
    template <>
    struct IsEmpty<Typelist<>> {
      static constexpr bool value = true;
    };
    
    template<typename Elements, template <typename T> class Func, bool = IsEmpty<Elements>::value>
    class TransformT ;
    
    template<typename... Elements, template<typename T> class MetaFun>
    class TransformT<Typelist < Elements...> , MetaFun, false> {
    public:
      using Type = Typelist<typename MetaFun<Elements>::Type...>;
    };
  
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
//main_ch24_4()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch24_4::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch24_4::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch24_4::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch24_4::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch24_4::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;
  
  return 0;
}