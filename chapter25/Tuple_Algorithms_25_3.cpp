//
// Created by yangfeng on 9/20/2022.
//

#include "../headers.h"

using namespace std;

/*
 25.3 元组的算法
元组是一种提供了以下各种功能的容器：可以访问并修改其元素的能力（通过 get<>），创
建新元组的能力（直接创建或者通过使用 makeTuple<>创建），以及将元组分割成 head 和
tail 的能力（通过使用 getHead()和 getTail()）。使用这些功能足以创建各种各样的元组算法，
比如添加或者删除元组中的元素，重新排序元组中的元素，或者选取元组中元素的某些子集。
元组很有意思的一点是它既需要用到编译期计算也需要用到运行期计算。和第 24 章介绍的
类型列表算法类似，将某种算法作用与元组之后可能会得到一个类型迥异的元组，这就需要
用到编译期计算。比如反转元组 Tuple<int, double, string>会得到 Tuple<string, double, int>。
五车书馆
348
但是和同质容器的算法类似（比如作用域 std::vector 的 std::reverse()），元组算法是需要在
运行期间执行代码的，因此我们需要留意被产生出来的代码的效率问题。
25.3.1 将元组用作类型列表
如果我们忽略掉 Tuple 模板在运行期间的相关部分，可以发现它在结构上和第 24 章介绍的
Typelist 完全一样：都接受任意数量的模板类型参数。事实上，通过使用一些部分特例化，
可以将 Tuple 变成一个功能完整的 Typelist：
// determine whether the tuple is empty:
template<>
struct IsEmpty<Tuple<>> {
static constexpr bool value = true;
};
// extract front element:
template<typename Head, typename… Tail>
class FrontT<Tuple<Head, Tail…>> {
public:
using Type = Head;
};
// remove front element:
template<typename Head, typename… Tail>
class PopFrontT<Tuple<Head, Tail…>> {
public:
using Type = Tuple<Tail…>;
};
// add element to the front:
template<typename… Types, typename Element>
class PushFrontT<Tuple<Types…>, Element> {
public:
using Type = Tuple<Element, Types…>;
};
// add element to the back:
template<typename… Types, typename Element>
class PushBackT<Tuple<Types…>, Element> {
public:
using Type = Tuple<Types…, Element>;
};
现在，所有在第 24 章开发的 typlist 算法都既适用于 Tuple 也适用于 Typelist，这样就可以很
五车书馆
349
方便的处理元组的类型了。比如：
Tuple<int, double, std::string> t1(17, 3.14, "Hello, World!");
using T2 = PopFront<PushBack<decltype(t1), bool>>;
T2 t2(get<1>(t1), get<2>(t1), true);
std::cout << t2;
会打印出：
(3.14, Hello, World!, 1)
很快就会看到，将 typelist 算法用于 tuple，通常是为了确定 tuple 算法返回值的类型。
25.3.2 添加以及删除元素
对于 Tuple，能否向其头部或者尾部添加元素，对开发相关的高阶算法而言是很重要的。和
typelist 的情况一样，向头部插入一个元素要远比向尾部插入一个元素要简单，因此我们从
pushFront 开始：
template<typename… Types, typename V>
PushFront<Tuple<Types…>, V>
pushFront(Tuple<Types…> const& tuple, V const& value)
{
return PushFront<Tuple<Types…>, V>(value, tuple);
}
将一个新元素（称之为 value）添加到一个已有元组的头部，需要生成一个新的、以 value
为 head、以已有 tuple 为 tail 的元组。返回结过的类型是 Tuple<V, Types…>。不过这里我们
选择使用 typelist 的算法 PushFront 来获得返回类型，这样做可以体现出 tuple 算法中编译期
部分和运行期部分之间的紧密耦合关系：编译期的 PushFront 计算出了我们应该生成的运行
期结果的类型。
将一个新元素添加到一个已有元组的末尾则会复杂得多，因为这需要遍历一个元组。注意下
面的代码中 pushBack()的实现方式，是如何参考了第 24.2.3 节中类型列表的 PushBack()的递
归实现方式的：
// basis case
template<typename V>
Tuple<V> pushBack(Tuple<> const&, V const& value)
{
return Tuple<V>(value);
}
// recursive case
template<typename Head, typename… Tail, typename V>
Tuple<Head, Tail…, V>
pushBack(Tuple<Head, Tail…> const& tuple, V const& value)
五车书馆
350
{
return Tuple<Head, Tail…, V>(tuple.getHead(),
pushBack(tuple.getTail(), value));
}
对于基本情况，和预期的一样，会将值追加到一个长度为零的元组的后面。对于递归情况，
则将元组分为 head 和 tail 两部分，然后将首元素以及将新元素追加到 tail 的后面得到结果组
装成最终的结果。虽然这里我们使用的返回值类型是 Tuple<Head, Tail…, V> ，但是它和编译
期的 PushBack<Tuple<Hrad, Tail...>, V>是一样的。
同样地，popFront()也很容易实现：
template<typename… Types>
PopFront<Tuple<Types…>> popFront(Tuple<Types…> const& tuple)
{
return tuple.getTail();
}
现在我们可以像下面这样编写第 25.3.1 节的例子：
Tuple<int, double, std::string> t1(17, 3.14, "Hello, World!");
auto t2 = popFront(pushBack(t1, true));
std::cout << std::boolalpha << t2 << ’\n’;
打印结果为：
(3.14, Hello, World!, true)
25.3.3 元组的反转
元组的反转可以采用另一种递归的、类似在第 24.2.4 节介绍的、类型列表的反转方式实现：
// basis case
Tuple<> reverse(Tuple<> const& t)
{
return t;
}
// recursive case
template<typename Head, typename… Tail>
Reverse<Tuple<Head, Tail…>> reverse(Tuple<Head, Tail…> const& t)
{
return pushBack(reverse(t.getTail()), t.getHead());
}
基本情况比较简单，而递归情况则是递归地将 head 追加到反转之后的 tail 的后面。也就是
说：
五车书馆
351
reverse(makeTuple(1, 2.5, std::string("hello")))
会生成一个包含了 string(“hello”)，2.5，和 1 的类型为 Tuple<string, double, int>的元组。
和类型列表类似，现在就可以简单地通过先反转元组，然后调用 popFront()，然后再次反转
元组实现 popBack():
template<typename… Types>
PopBack<Tuple<Types…>> popBack(Tuple<Types…> const& tuple){
return reverse(popFront(reverse(tuple)));
}
25.3.4 索引列表
虽然上文中反转元组用到的递归方式是正确的，但是它在运行期间的效率却非常低。为了展
现这一问题，引入下面这个可以计算其实例被 copy 次数的类：
template<int N>
struct CopyCounter
{
inline static unsigned numCopies = 0;
CopyCounter()
{
}
CopyCounter(CopyCounter const&) {
++numCopies;
}
};
然后创建并反转一个包含了 CopyCounter 实例的元组：
void copycountertest()
{
Tuple<CopyCounter<0>, CopyCounter<1>, CopyCounter<2>, CopyCounter<3>,
CopyCounter<4>> copies;
auto reversed = reverse(copies);
std::cout << "0: " << CopyCounter<0>::numCopies << " copies\n";
std::cout << "1: " << CopyCounter<1>::numCopies << " copies\n";
std::cout << "2: " << CopyCounter<2>::numCopies << " copies\n";
std::cout << "3: " << CopyCounter<3>::numCopies << " copies\n";
std::cout << "4: " << CopyCounter<4>::numCopies << " copies\n";
}
这个程序会打印出：
0: 5 copies
1: 8 copies
五车书馆
352
2: 9 copies
3: 8 copies
4: 5 copies
这确实进行了很多次 copy！在理想的实现中，反转一个元组时，每一个元素只应该被 copy
一次：从其初始位置直接被 copy 到目的位置。我们可以通过使用引用来达到这一目的，包
括对中间变量的类型使用引用，但是这样做会使实现变得很复杂。
在反转元组时，为了避免不必要的 copy，考虑一下我们该如何实现一个一次性的算法，来
反转一个简单的、长度已知的元组（比如包含 5 个元素）。可以像下面这样只是简单地使用
makeTuple()和 get():
auto reversed = makeTuple(get<4>(copies), get<3>(copies), get<2>(copies),
get<1>(copies), get<0>(copies));
这个程序会按照我们预期的那样进行，对每个元素只进行一次 copy：
0: 1 copies
1: 1 copies
2: 1 copies
3: 1 copies
4: 1 copies
索引列表（亦称索引序列，参见第 24.4 节）通过将一组元组的索引捕获进一个参数包，推
广了上述概念，本例中的索引列表是 4，3，2，1，0，这样就可以通过包展开进行一组 get
函数的调用。采用这种方法可以将索引列表的计算（可以采用任意复杂度的模板源程序）和
使用（更关注运行期的性能）分离开。在 C++14 中引入的标准类型 std::integer_sequence，
通常被用来表示索引列表。
25.3.5 通过索引列表进行反转
为了将索引列表用于元组反转，我们首先要找到一种能够表达索引列表的方式。索引列表是
一种包含了数值的类型列表，这些数值被用作指向另一个类型列表或者异质容器（参见 25.4
节）的索引。此处我们将第 24.3 节介绍的 Valuelist 用作类型列表。上文例子中反转元组时
用到的索引列表可以被写成：
Valuelist<unsigned, 4, 3, 2, 1, 0>
那么该如何生成一个索引列表呢？一种方式是使用下面的这个简单的模板元函数
MakeIndexList，它从 0 到 N-1（N 是元组长度）逐步生成索引列表：
// recursive case
template<unsigned N, typename Result = Valuelist<unsigned>>
struct MakeIndexListT
: MakeIndexListT<N-1, PushFront<Result, CTValue<unsigned, N-1>>>
{};
五车书馆
353
// basis case
template<typename Result>
struct MakeIndexListT<0, Result>
{
using Type = Result;
};
template<unsigned N>
using MakeIndexList = typename MakeIndexListT<N>::Type;
现在就可以结合 MakeIndexList 和在第 24.2.4 节介绍的类型列表的 Reverse 算法，生成所需的
索引列表：
using MyIndexList = Reverse<MakeIndexList<5>>;
// equivalent to Valuelist<unsigned, 4, 3, 2,1, 0>
为了真正实现反转，需要将索引列表中的索引捕获进一个非类型参数包。这可以通过将
reverse()分成两部分来实现：
template<typename… Elements, unsigned… Indices>
auto reverseImpl(Tuple<Elements…> const& t, Valuelist<unsigned, Indices…>)
{
return makeTuple(get<Indices>(t)…);
}
template<typename… Elements>
auto reverse(Tuple<Elements…> const& t)
{
return reverseImpl(t, Reverse<MakeIndexList<sizeof…(Elements)>>());
}
在 C++11 中相应的返回类型要通过尾置返回类型声明：
-> decltype(makeTuple(get<Indices>(t)…))
和：
-> decltype(reverseImpl(t, Reverse<MakeIndexList<sizeof… (Elements)>>()))
其中函数模板 reverseImpl()从其参数 Valuelist 中捕获相应的索引信息，并将之存储进参数包
Indices 中。然后以 get<Indices>(t)…为参数调用 makeTuple()，并生成返回结果。
而 reverse()所做的只是生成合适的索引组，然后以之为参数调用 reverseImpl。这里用模板元
程序操作索引列表，因此不会生成任何运行期间的代码。唯一的运行期代码是 reverseImpl，
它通过调用 makeTuple()，只用一步就生成了最终的结果，而且只对元组中的元素进行了一
次 copy。
五车书馆
354
25.3.6 洗牌和选择（Shuffle and Select ）
事实上，上一节中为了反转元组而用到的函数模板 reverseImpl()，并不是仅适用于 reverse()。
它所做的只是从一个已有元组中选出一组特定的值，并用它们生成一个新的元组。虽然
reverse()提供的是一组反序的索引，但是其它一些算法可以通过提供一组自己的索引来使用
下面的 select()算法：
template<typename… Elements, unsigned… Indices>
auto select(Tuple<Elements…> const& t, Valuelist<unsigned, Indices…>)
{
return makeTuple(get<Indices>(t)…);
}
一个使用了 select()的简单算法是“splat”，它从元组中选出一个元素，将之重复若干次之
后组成一个新的元组。比如：
Tuple<int, double, std::string> t1(42, 7.7, "hello"};
auto a = splat<1, 4>(t);
std::cout << a << ’\n’;
它会生成一个 Tuple<double, double, double, double>类型的元组，其每一个值都是 get<1>(t)
的一份 copy，因此最终打印的结果是：
(7.7, 7.7, 7.7, 7.7)
在提供了一个能够生成一组重复索引（N 个 I）的元程序后，就可以直接用 select()实现 splat()：
template<unsigned I, unsigned N, typename IndexList = Valuelist<unsigned>>
class ReplicatedIndexListT;
template<unsigned I, unsigned N, unsigned… Indices>
class ReplicatedIndexListT<I, N, Valuelist<unsigned, Indices…>>
: public ReplicatedIndexListT<I, N-1, Valuelist<unsigned, Indices…, I>>
{ };
template<unsigned I, unsigned… Indices>
class ReplicatedIndexListT<I, 0, Valuelist<unsigned, Indices…>> {
public:
using Type = Valuelist<unsigned, Indices…>;
};
template<unsigned I, unsigned N>
using ReplicatedIndexList = typename ReplicatedIndexListT<I, N>::Type;
template<unsigned I, unsigned N, typename… Elements>
auto splat(Tuple<Elements…> const& t)
{
五车书馆
355
return select(t, ReplicatedIndexList<I, N>());
}
即使是更复杂的元组算法，也可以通过使用 select()函数和一个操作索引列表的模板元函数
实现。比如，可以用在第 24.2.7 节开发的插入排序算法，基于元素类型的大小对元组进行排
序。假设有这样一个 sort()函数，它接受一个用来比较元组元素类型的模板元函数作为参数，
就可以按照下面的方式对元组进行排序：
#include <complex>
template<typename T, typename U>
class SmallerThanT
{
public:
static constexpr bool value = sizeof(T) < sizeof(U);
};
void testTupleSort()
{
auto T1 = makeTuple(17LL, std::complex<double>(42,77), ’c’, 42, 7.7);
std::cout << t1 << ’\n’;
auto T2 = sort<SmallerThanT>(t1); // t2 is Tuple<int, long,
std::string>
std::cout << "sorted by size: " << t2 << ’\n’;
}
输出结果如下：
(17, (42,77), c, 42, 7.7)
sorted by size: (c, 42, 7.7, 17, (42,77))
sort()的具体实现使用了 InsertionSort 和 select()：
// metafunction wrapper that compares the elements in a tuple:
template<typename List, template<typename T, typename U> class F>
class MetafunOfNthElementT {
public:
template<typename T, typename U>
class Apply;
template<unsigned N, unsigned M>
class Apply<CTValue<unsigned, M>, CTValue<unsigned, N>>
: public F<NthElement<List, M>, NthElement<List, N>>
{ };
};
// sort a tuple based on comparing the element types:
template<template<typename T, typename U> class Compare, typename…
五车书馆
356
Elements>
auto sort(Tuple<Elements…> const& t)
{
return select(t, InsertionSort<MakeIndexList<sizeof…(Elements)>,
MetafunOfNthElementT<Tuple<Elements…>,
Compare>::template Apply>());
}
注意 InsertionSort 的使用：真正被排序的类型列表是一组指向类型列表的索引，该索引通过
MakeIndexList<>构造。因此插入排序的结果是一组指向元组的索引，并被传递给 selete()使
用。不过由于 InsertionSort 被用来操作索引，它所期望的比较操作自然也是比较两个索引。
考虑一下对一个 std::vector 的索引进行排序的情况，就很容易理解背后的相关原理了，比如
下面的这个（非元编程）例子：
#include <vector>
#include <algorithm>
#include <string>
int main()
{
std::vector<std::string> strings = {"banana", "apple", "cherry"};
std::vector<unsigned> indices = { 0, 1, 2 };
std::sort(indices.begin(), indices.end(),
[&strings](unsigned i, unsigned j) {
return strings[i] < strings[j];
}
);
}
这里变量 indices 包含的是指向变量 strings 的索引。sort()函数对索引进行排序，它用到了一
个接受两个 unsigned 类型的数值作为参数的 lambda 比较函数。但是由于 lambda 函数的主
体将 unsigned 的数值当作 strings 变量的索引处理，因此真正被排序的还是 strings 的内容。
在排序的最后，变量 indices 包含的依然是指向 strings 的索引，只是这个索引是按照 strings
的值进行排序之后的索引。
我们在代码中将 InsertionSort 用于元组的 sort()函数，情况和上面的例子是一样的。在适配
模板 MetafuncOfNthElementT 中提供了一个接受两个索引作为参数的模板元函数（Apply()），
而它又会使用 NthElement 从其 Typelist 参数中提取相应的元素。在某种意义上，成员模板
Apply 捕获了提供给其外层模板（MetafunOfNthElementT）的类型列表参数，这和 lambda
函数捕获其外层作用域中的 strings vector 的情况类似。然后 Apply 将其提取的元素类型转发
给底层的元函数 F，并结束适配。
注意上文中所有排序相关的计算都发生在编译期间，作为结果的元素也是直接生成的，不会
用到运行期间的拷贝。
 */

namespace ch25_3 {

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
//main()
main_ch25_3()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch25_3::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch25_3::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch25_3::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch25_3::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch25_3::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}