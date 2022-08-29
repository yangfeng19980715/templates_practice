//
// Created by yangfeng on 2022/8/25.
//

#include "../headers.h"

using namespace std;

/*
第 21 章 模板和继承
直觉上，模板和继承之间似乎并不应该存在什么有意思的交互。如果有的话，那么也应该是
在第 13 章中介绍的，当从一个和模板参数有关的基类做继承的时候，必须仔细地对待那些
不受限制的变量名。但是事实证明，一些有意思的技术恰恰结合了这两种技术，比如 Curiously
Recurring Template Pattern（CRTP）和 MIXINS。本章将介绍其中的一些相关技术。
21.1 空 基 类 优 化 （ The Empty Class Optimization ，
EBCO）
C++中的类经常是“空”的，也就是说它们的内部表征在运行期间不占用内存。典型的情况
是那写只包含类型成员，非虚成员函数，以及静态数据成员的类。而非静态数据成员，虚函
数，以及虚基类，在运行期间则是需要占用内存的。
然而即使是空的类，其所占用的内存大小也不是零。如果愿意的话，运行下面的程序可以证
明这一点：
#include <iostream>
class EmptyClass {
};
int main()
{
std::cout << "sizeof(EmptyClass):" << sizeof(EmptyClass) << ’\n’;
}
在某些平台上，这个程序会打印出 1。在少数对 class 类型实施了严格内存对齐要求的平台
上，则可能会打印出其它结果（典型的结果是 4）.
21.1.1 布局原则
C++的设计者有很多种理由不去使用内存占用为零的 class。比如，一个存储了内存占用为零
的 class 的数组，其内存占用也将是零，这样的话常规的指针运算规则都将不在适用。假设
ZeroSizedT 是一个内存占用为零的类型：
ZeroSizedT z[10];
…
&z[i] - &z[j] //compute distance between pointers/addresses
五车书馆
268
正常情况下，上述例子中的结果可以用两个地址之间的差值，除以该数组中元素类型的大小
得到，但是如果元素所占用内存为零的话，上述结论显然不再成立。
虽然在 C++中没有内存占用为零的类型，但是 C++标准却指出，在空 class 被用作基类的时候，
如果不给它分配内存并不会导致其被存储到与其它同类型对象或者子对象相同的地址上，那
么就可以不给它分配内存。下面通过一些例子来看看实际应用中空基类优化（empty class
optimization，EBCO）的意义。考虑如下程序：
#include <iostream>
class Empty {
using Int = int;// type alias members don’t make a class nonempty
};
class EmptyTwo : public Empty {
};
class EmptyThree : public EmptyTwo {
}；
int main()
{
std::cout << "sizeof(Empty): " << sizeof(Empty) << ’\n’;
std::cout << "sizeof(EmptyTwo): " << sizeof(EmptyTwo) << ’\n’;
std::cout << "sizeof(EmptyThree): " << sizeof(EmptyThree) << ’\n’;
}
如果你所使用的编译器实现了 EBCO 的话，它打印出来的三个 class 的大小将是相同的，但
是它们的结果也都不会是零（参见图 21.1）。这意味着在 EmptyTwo 中，Empty 没有被分配
内存。注意一个继承自优化后的空基类（且只有这一个基类）的空类依然是空的。这就解释
了为什么 EmptyThree 的大小和 Empty 相同。如果你所用的编译器没有实现 EBCO 的话，那
么它打印出来的各个 class 的大小将是不同的（参见图 21.2）。
Figure 21.1. Layout of EmptyThree by a compiler that implements the EBCO
五车书馆
269
Figure 21.2. Layout of EmptyThree by a compiler that does not implement the EBCO
考虑一种 EBCO 不适用的情况：
#include <iostream>!
class Empty {
using Int = int; // type alias members don’t make a class nonempty
};
class EmptyTwo : public Empty {
};
class NonEmpty : public Empty, public EmptyTwo {
};
int main(){
std::cout <<"sizeof(Empty): " << sizeof(Empty) <<’\n’;
std::cout <<"sizeof(EmptyTwo): " << sizeof(EmptyTwo) <<’\n’;
std::cout <<"sizeof(NonEmpty): " << sizeof(NonEmpty) <<’\n’;
}
可能有点意外的是，NonEmpty 不再是一个空的类。毕竟它以及它的基类都没有任何数据成
员。但是 NonEmpty 的基类 Empty 和 EmptyTwo 不可以被分配到相同的地址上，因为这会导
致 EmptyTwo 的基类 Empty 和 NonEmpty 的基类 Empty 被分配到相同的地址。或者说两个类
型相同的子对象会被分配到相同的地址上，而这在 C++布局规则中是不被允许的。你可能会
想到将其中一个 Empty 基类的子对象放在偏移量为“0 字节”的地方，将另一个放在偏移量
为“1 字节”的地方，但是完整的 NonEmpty 对象的内存占用依然不能是 1 字节，因为在一
个包含了两个 NonEmpty 对象的数组中，第一个元素的 Empty 子对象不能和第二个元素中的
Empty 子对象占用相同的地址（参见图 21.3）。
五车书馆
270
Figure 21.3. Layout of NonEmpty by a compiler that implements the EBCO
EBCO 之所以会有这一限制，是因为我们希望能够通过比较两个指针来确定它们所指向的是
不是同一个对象。由于指针在程序中几乎总是被表示为单纯的地址，因此就需要我们来确保
两个不同的地址（比如指针的值）指向的总是两个不同的对象。
这一限制可能看上去并不是那么重要。但是，在实践中却经常遇到，因为有些类会倾向于从
一组空的、定义了某些基本类型别名的类做继承。当两个这一类 class 的子对象被用于同一
个完整类型中的时候，这一优化方案会被禁止。
即使有这一限制，对于模板库而言 EBCO 也依然是一个重要的优化方案，因为有很多技术在
引入基类的时候都只是为了引入一些新的类型别名或者额外的函数功能，而不会增加新的数
据成员。在本章中会对其中些这一类的技术进行讨论。
21.1.2 将数据成员实现为基类
EBCO 和数据成员之间没有对等关系，因为（其中一个问题是）它会在用指针指向数据成员
的表示上造成一些问题。结果就是，在有些情况下会期望将其实现为一个 private 的基类，
这样粗看起来就可以将其视作成员变量。但是，这样做也并不是没有问题。
由于模板参数经常会被空 class 类型替换，因此在模板上下文中这一问题要更有意思一些，
但是通常我们不能依赖这一规则。如果我们对类型参数一无所知，就不能很容易的使用
EBCO。考虑下面的例子：
template<typename T1, typename T2>
class MyClass {
private:
T1 a;
T2 b;
…
};
其中的一个或者两个模板参数完全有可能被空 class 类型替换。如果真是这样，那么
MyClass<T1, T2>这一表达方式可能不是最优的选择，它可能会为每一个 MyClass<T1,T2>的实
例都浪费一个字的内存。
这一内存浪费可以通过把模板参数作为基类使用来避免：
五车书馆
271
template<typename T1, typename T2>
class MyClass : private T1, private T2 {
};
但是这一直接的替代方案也有其自身的缺点：

当 T1 或者 T2 被一个非 class 类型或者 union 类型替换的时候，该方法不再适用。

在两个模板参数被同一种类型替换的时候，该方法不再适用（虽然这一问题简单地通过
增加一层额外的继承来解决，参见 513 页）。

用来替换 T1 或者 T2 的类型可能是 final 的，此时尝试从其派生出新的类会触发错误。
即使这些问题能够很好的解决，也还有一个严重的问题存在：给一个 class 添加一个基类，
可能会从根本上改变该 class 的接口。对于我们的 MyClass 类，由于只有很少的接口会被影
响到，这可能看上去不是一个重要的问题。但是正如在本章接下来的内容中将要看到的，从
一个模板参数做继承，会影响到一个成员函数是否可以是 virtual 的。很显然，EBCO 的这一
适用方式会带来各种各样的问题。
当已知模板参数只会被 class 类型替换，以及需要支持另一个模板参数的时候，可以使用另
一种更实际的方法。其主要思想是通过使用 EBCO 将可能为空的类型参数与别的参数“合并”。
比如，相比于这样：
template<typename CustomClass>
class Optimizable {
private:
CustomClass info; // might be empty
void* storage;
…
};
一个模板开发者会使用如下方式：
template<typename CustomClass>
class Optimizable {
private:
BaseMemberPair<CustomClass, void*> info_and_storage;
…
};
虽然还没有看到 BaseMemberPari 的具体实现方式，但是可以肯定它的引入会使 Optimizable
的实现变得更复杂。但是很多的模板开发者都反应，相比于复杂度的增加，它带来的性能提
升是值得的。我们会在第 25.5.1 节对这一内容做进一步讨论。
BaseMemberPair 的实现可以非常简洁：
#ifndef BASE_MEMBER_PAIR_HPP
#define BASE_MEMBER_PAIR_HPP
template<typename Base, typename Member>
class BaseMemberPair : private Base {
五车书馆
272
private:
Member mem;
public:// constructor
BaseMemberPair (Base const & b, Member const & m)
: Base(b), mem(m) {
}
// access base class data via first()
Base const& base() const {
return static_cast<Base const&>(*this);
}
Base& base() {
return static_cast<Base&>(*this);
}
// access member data via second()
Member const& member() const {
return this->mem;
}
Member& member() {
return this->mem;
}
};
#endif // BASE_MEMBER_PAIR_HPP
相应的实现需要使用 base()和 member()成员函数来获取被封装的（或者被执行了内存优化
的）数据成员。
 */

namespace ch21_1 {
  
  /*
   第 21 章 模板和继承
直觉上，模板和继承之间似乎并不应该存在什么有意思的交互。如果有的话，那么也应该是
在第 13 章中介绍的，当从一个和模板参数有关的基类做继承的时候，必须仔细地对待那些
不受限制的变量名。但是事实证明，一些有意思的技术恰恰结合了这两种技术，比如 Curiously Recurring Template Pattern（CRTP） (奇异递归模板)
   和 MIXINS。本章将介绍其中的一些相关技术。
   */

  namespace case1 {
    void test() {
      cout << "hello, world" << endl;
    }
  }
  
  /*
  21.1 空 基 类 优 化 （ The Empty Class Optimization ，
EBCO）
C++中的类经常是“空”的，也就是说它们的内部表征在运行期间不占用内存。典型的情况
是那写只包含类型成员，非虚成员函数，以及静态数据成员的类。而非静态数据成员，虚函
数，以及虚基类，在运行期间则是需要占用内存的。
然而即使是空的类，其所占用的内存大小也不是零。如果愿意的话，运行下面的程序可以证
明这一点：
#include <iostream>
class EmptyClass {
};
int main()
{
std::cout << "sizeof(EmptyClass):" << sizeof(EmptyClass) << ’\n’;
}
在某些平台上，这个程序会打印出 1。在少数对 class 类型实施了严格内存对齐要求的平台
上，则可能会打印出其它结果（典型的结果是 4）.
   */
  namespace case2 {
    class EmptyClass { };
    // class alignas(4) EmptyClass { };
    
    void test() {
      cout << "sizeof(EmptyClass) : " << sizeof(EmptyClass) << endl;
    }
    
  }
  
  /*
   21.1.1 布局原则
C++的设计者有很多种理由不去使用内存占用为零的 class。比如，一个存储了内存占用为零
的 class 的数组，其内存占用也将是零，这样的话常规的指针运算规则都将不在适用。假设
ZeroSizedT 是一个内存占用为零的类型：
ZeroSizedT z[10];
…
&z[i] - &z[j] //compute distance between pointers/addresses
五车书馆
268
正常情况下，上述例子中的结果可以用两个地址之间的差值，除以该数组中元素类型的大小
得到，但是如果元素所占用内存为零的话，上述结论显然不再成立。
虽然在 C++中没有内存占用为零的类型，但是 C++标准却指出，在空 class 被用作基类的时候，
如果不给它分配内存并不会导致其被存储到与其它同类型对象或者子对象相同的地址上，那
么就可以不给它分配内存。下面通过一些例子来看看实际应用中空基类优化（empty class
optimization，EBCO）的意义。考虑如下程序：
#include <iostream>
class Empty {
using Int = int;// type alias members don’t make a class nonempty
};
class EmptyTwo : public Empty {
};
class EmptyThree : public EmptyTwo {
}；
int main()
{
std::cout << "sizeof(Empty): " << sizeof(Empty) << ’\n’;
std::cout << "sizeof(EmptyTwo): " << sizeof(EmptyTwo) << ’\n’;
std::cout << "sizeof(EmptyThree): " << sizeof(EmptyThree) << ’\n’;
}
如果你所使用的编译器实现了 EBCO 的话，它打印出来的三个 class 的大小将是相同的，但
是它们的结果也都不会是零（参见图 21.1）。这意味着在 EmptyTwo 中，Empty 没有被分配
内存。注意一个继承自优化后的空基类（且只有这一个基类）的空类依然是空的。这就解释
了为什么 EmptyThree 的大小和 Empty 相同。如果你所用的编译器没有实现 EBCO 的话，那
么它打印出来的各个 class 的大小将是不同的（参见图 21.2）。
Figure 21.1. Layout of EmptyThree by a compiler that implements the EBCO
五车书馆
269
Figure 21.2. Layout of EmptyThree by a compiler that does not implement the EBCO
考虑一种 EBCO 不适用的情况：
#include <iostream>!
class Empty {
using Int = int; // type alias members don’t make a class nonempty
};
class EmptyTwo : public Empty {
};
class NonEmpty : public Empty, public EmptyTwo {
};
int main(){
std::cout <<"sizeof(Empty): " << sizeof(Empty) <<’\n’;
std::cout <<"sizeof(EmptyTwo): " << sizeof(EmptyTwo) <<’\n’;
std::cout <<"sizeof(NonEmpty): " << sizeof(NonEmpty) <<’\n’;
}
可能有点意外的是，NonEmpty 不再是一个空的类。毕竟它以及它的基类都没有任何数据成
员。但是 NonEmpty 的基类 Empty 和 EmptyTwo 不可以被分配到相同的地址上，因为这会导
致 EmptyTwo 的基类 Empty 和 NonEmpty 的基类 Empty 被分配到相同的地址。或者说两个类
型相同的子对象会被分配到相同的地址上，而这在 C++布局规则中是不被允许的。你可能会
想到将其中一个 Empty 基类的子对象放在偏移量为“0 字节”的地方，将另一个放在偏移量
为“1 字节”的地方，但是完整的 NonEmpty 对象的内存占用依然不能是 1 字节，因为在一
个包含了两个 NonEmpty 对象的数组中，第一个元素的 Empty 子对象不能和第二个元素中的
Empty 子对象占用相同的地址（参见图 21.3）。
五车书馆
270
Figure 21.3. Layout of NonEmpty by a compiler that implements the EBCO
EBCO 之所以会有这一限制，是因为我们希望能够通过比较两个指针来确定它们所指向的是
不是同一个对象。由于指针在程序中几乎总是被表示为单纯的地址，因此就需要我们来确保
两个不同的地址（比如指针的值）指向的总是两个不同的对象。
这一限制可能看上去并不是那么重要。但是，在实践中却经常遇到，因为有些类会倾向于从
一组空的、定义了某些基本类型别名的类做继承。当两个这一类 class 的子对象被用于同一
个完整类型中的时候，这一优化方案会被禁止。
即使有这一限制，对于模板库而言 EBCO 也依然是一个重要的优化方案，因为有很多技术在
引入基类的时候都只是为了引入一些新的类型别名或者额外的函数功能，而不会增加新的数
据成员。在本章中会对其中些这一类的技术进行讨论。
   */
  
  namespace case3 {
      class Empty {
        using Int = int;// type alias members don’t make a class nonempty
      };
      
      class EmptyTwo : public Empty { };
      
      class EmptyThree : public EmptyTwo { };
      void test()
      {
        std::cout << "sizeof(Empty): " << sizeof(Empty) << endl;
        std::cout << "sizeof(EmptyTwo): " << sizeof(EmptyTwo) << endl;
        std::cout << "sizeof(EmptyThree): " << sizeof(EmptyThree) << endl;
      }
  }
  
  namespace case4 {
    class Empty {
      using Int = int; // type alias members don’t make a class nonempty
    };
    
    class EmptyToo : public Empty { };
    
    // class NonEmpty : public Empty, public EmptyToo { };
    
    void test(){
      std::cout <<"sizeof(Empty): " << sizeof(Empty)       << endl;
      std::cout <<"sizeof(EmptyToo): " << sizeof(EmptyToo) << endl;
      // std::cout <<"sizeof(NonEmpty): " << sizeof(NonEmpty) << endl;
    }
    
  }
  
  /*
  21.1.2 将数据成员实现为基类
EBCO 和数据成员之间没有对等关系，因为（其中一个问题是）它会在用指针指向数据成员
的表示上造成一些问题。结果就是，在有些情况下会期望将其实现为一个 private 的基类，
这样粗看起来就可以将其视作成员变量。但是，这样做也并不是没有问题。
由于模板参数经常会被空 class 类型替换，因此在模板上下文中这一问题要更有意思一些，
但是通常我们不能依赖这一规则。如果我们对类型参数一无所知，就不能很容易的使用
EBCO。考虑下面的例子：
template<typename T1, typename T2>
class MyClass {
private:
T1 a;
T2 b;
…
};
其中的一个或者两个模板参数完全有可能被空 class 类型替换。如果真是这样，那么
MyClass<T1, T2>这一表达方式可能不是最优的选择，它可能会为每一个 MyClass<T1,T2>的实
例都浪费一个字的内存。
这一内存浪费可以通过把模板参数作为基类使用来避免：
五车书馆
271
template<typename T1, typename T2>
class MyClass : private T1, private T2 {
};
但是这一直接的替代方案也有其自身的缺点：

当 T1 或者 T2 被一个非 class 类型或者 union 类型替换的时候，该方法不再适用。

在两个模板参数被同一种类型替换的时候，该方法不再适用（虽然这一问题简单地通过
增加一层额外的继承来解决，参见 513 页）。

用来替换 T1 或者 T2 的类型可能是 final 的，此时尝试从其派生出新的类会触发错误。
即使这些问题能够很好的解决，也还有一个严重的问题存在：给一个 class 添加一个基类，
可能会从根本上改变该 class 的接口。对于我们的 MyClass 类，由于只有很少的接口会被影
响到，这可能看上去不是一个重要的问题。但是正如在本章接下来的内容中将要看到的，从
一个模板参数做继承，会影响到一个成员函数是否可以是 virtual 的。很显然，EBCO 的这一
适用方式会带来各种各样的问题。
当已知模板参数只会被 class 类型替换，以及需要支持另一个模板参数的时候，可以使用另
一种更实际的方法。其主要思想是通过使用 EBCO 将可能为空的类型参数与别的参数“合并”。
比如，相比于这样：
template<typename CustomClass>
class Optimizable {
private:
CustomClass info; // might be empty
void* storage;
…
};
一个模板开发者会使用如下方式：
template<typename CustomClass>
class Optimizable {
private:
BaseMemberPair<CustomClass, void*> info_and_storage;
…
};
虽然还没有看到 BaseMemberPari 的具体实现方式，但是可以肯定它的引入会使 Optimizable
的实现变得更复杂。但是很多的模板开发者都反应，相比于复杂度的增加，它带来的性能提
升是值得的。我们会在第 25.5.1 节对这一内容做进一步讨论。
BaseMemberPair 的实现可以非常简洁：
#ifndef BASE_MEMBER_PAIR_HPP
#define BASE_MEMBER_PAIR_HPP
template<typename Base, typename Member>
class BaseMemberPair : private Base {
五车书馆
272
private:
Member mem;
public:// constructor
BaseMemberPair (Base const & b, Member const & m)
: Base(b), mem(m) {
}
// access base class data via first()
Base const& base() const {
return static_cast<Base const&>(*this);
}
Base& base() {
return static_cast<Base&>(*this);
}
// access member data via second()
Member const& member() const {
return this->mem;
}
Member& member() {
return this->mem;
}
};
#endif // BASE_MEMBER_PAIR_HPP
相应的实现需要使用 base()和 member()成员函数来获取被封装的（或者被执行了内存优化
的）数据成员。
   */
  
  namespace case5 {
    template<typename CustomClass>
    class OptimizableA {
    private:
      CustomClass info; // might be empty
      void* storage;
    };
  
    template<typename Base, typename Member>
    class BaseMemberPair : private Base {
    private:
      Member mem;
    public:// constructor
      BaseMemberPair (Base const & b, Member const & m) : Base(b), mem(m) { }
    
      // access base class data via first()
      Base const& base() const { return static_cast<Base const&>(*this); }
    
      Base& base() { return static_cast<Base&>(*this); }
      
      // access member data via second()
      Member const& member() const { return this->mem; }
      
      Member& member() { return this->mem; }
    };
    
    template<typename CustomClass>
    class OptimizableB {
    private:
      BaseMemberPair<CustomClass, void*> info_and_storage;
    };
    
    void test() {
    
    }
  
  }

  class Tmp {
  public:
  
  };

}

int
//main()
main_ch21_1()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch21_1::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch21_1::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch21_1::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl;
  ch21_1::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl;
  ch21_1::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl;

  return 0;
}