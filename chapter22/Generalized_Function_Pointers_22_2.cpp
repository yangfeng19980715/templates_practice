//
// Created by yangfeng on 9/1/2022.
//

#include "../headers.h"

using namespace std;

/*
 22.2 广义函数指针
Std::functional<>类型是一种高效的、广义形式的 C++函数指针，提供了与函数指针相同的基
本操作：

在调用者对函数本身一无所知的情况下，可以被用来调用该函数。

可以被拷贝，move 以及赋值。

可以被另一个（函数签名一致的）函数初始化或者赋值。

如果没有函数与之绑定，其状态是“null”。
但是，与 C++函数指针不同的是，std::functional<>还可以被用来存储 lambda，以及其它任意
实现了合适的 operator()的函数对象，所有这些情况对应的类型都可能不同。
在本章接下来的内容中，我们会实现一版自己的广义函数指针类模板（FunctionPtr），会给
其提供相同的关键操作以及能力，并用之替换 std::functional:
#include "functionptr.hpp"
#include <vector>
#include <iostream>
void forUpTo(int n, FunctionPtr<void(int)> f)
{
for (int i = 0; i != n; ++i)
{
f(i); // call passed function f for i
}
}
void printInt(int i)
{
std::cout << i << ' ';
}
int main()
{
std::vector<int> values;
// insert values from 0 to 4:
forUpTo(5,[&values](int i) {
values.push_back(i);
});
// print elements:
五车书馆
296
forUpTo(5, printInt); // prints 0 1 2 3 4
std::cout << '\n';
}
FunctionPtr 的接口非常直观的提供了构造，拷贝，move，析构，初始化，以及从任意函数
对象进行赋值，还有就是要能够调用其底层的函数对象。接口中最有意思的一部分是如何在
一个类模板的偏特化中对其进行完整的描述，该偏特化将模板参数（函数类型）分解为其组
成部分（返回类型以及参数类型）：
// primary template:
template<typename Signature>
class FunctionPtr;
// partial specialization:
template<typename R, typename... Args>
class FunctionPtr<R(Args...)>
{
private:
FunctorBridge<R, Args...>* bridge;
public:
// constructors:
FunctionPtr() : bridge(nullptr) {
}
FunctionPtr(FunctionPtr const& other); // see
functionptrcpinv.hpp
FunctionPtr(FunctionPtr& other)
: FunctionPtr(static_cast<FunctionPtr const&>(other)) {
}
FunctionPtr(FunctionPtr&& other) : bridge(other.bridge) {
other.bridge = nullptr;
}
//construction from arbitrary function objects:
template<typename F> FunctionPtr(F&& f); // see
functionptrinit.hpp// assignment operators:
FunctionPtr& operator=(FunctionPtr const& other) {
FunctionPtr tmp(other);
swap(*this, tmp);
return *this;
}
FunctionPtr& operator=(FunctionPtr&& other) {
五车书馆
297
delete bridge;
bridge = other.bridge;
other.bridge = nullptr;
return *this;
}
//construction and assignment from arbitrary function objects:
template<typename F> FunctionPtr& operator=(F&& f) {
FunctionPtr tmp(std::forward<F>(f));
swap(*this, tmp);
return *this;
}
// destructor:
~FunctionPtr() {
delete bridge;
}
friend void swap(FunctionPtr& fp1, FunctionPtr& fp2) {
std::swap(fp1.bridge, fp2.bridge);
}
explicit operator bool() const {
return bridge == nullptr;
}
// invocation:
R operator()(Args... args) const; // see functionptr-cpinv.hpp
};
该实现包含了唯一一个非 static 的成员变量，bridge，它将负责被存储函数对象的储存和维
护。该指针的所有权被绑定到了一个 FunctionPtr 的对象上，因此相关的大部分实现都只需
要去操纵这个指针即可。代码中未被实现的、也是比较有意思的一部分，将在接下来的章节
中进行介绍。
 */

namespace ch22_2 {

  namespace case1 {
    
    class Callable {
    public:
      void operator()(int a ) {
        cout << __func__ << '\t' << "a: " << a << endl;
      }
    };
    
  
    void test() {
      std::function<void()> f =  []() { cout << "hello, lambda" << endl; };
      std::function<void(int)> f2 = Callable();
      
      f();
      f2(3);
    }
    
  }
  
  /*
   FunctionPtr 的接口非常直观的提供了构造，拷贝，move，析构，初始化，以及从任意函数
对象进行赋值，还有就是要能够调用其底层的函数对象。接口中最有意思的一部分是如何在
一个类模板的偏特化中对其进行完整的描述，该偏特化将模板参数（函数类型）分解为其组
成部分（返回类型以及参数类型）：
   */
  
  namespace case2 {
    
    /*
     FunctorBridge 类模板负责持有以及维护底层的函数对象，它被实现为一个抽象基类，为
FunctionPtr 的动态多态打下基础：
     */
    template<typename R, typename... Args>
    class FunctorBridge
    {
    public:
      /*
      上述 FunctionPtr 实现几乎可以被当作一个函数指针的非正式替代品适用。但是它并没有提
        供对下面这一函数指针操作的支持：检测两个 FunctionPtr 的对象是否会调用相同的函数。
      为了实现这一功能，需要在 FunctorBridge 中加入 equals 操作：
       */
      virtual bool equals(FunctorBridge<R, Args...> const* fb) const = 0;
      virtual ~FunctorBridge() { }
      virtual FunctorBridge* clone() const = 0;
      virtual R invoke(Args... args) const = 0;
    };
    
    /*
    FunctorBridge 通过虚函数提供了用来操作被存储函数对象的必要操作：一个析构函数，一个
用来执行 copy 的 clone()操作，以及一个用来调用底层函数对象的 invoke()操作。不要忘记将
clone()和 invoke()声明为 const 的成员函数。
有了这些虚函数，就可以继续实现 FunctionPtr 的拷贝构造函数和函数调用运算符了：
     */
    
    /*
     该实现包含了唯一一个非 static 的成员变量，bridge，它将负责被存储函数对象的储存和维
护。该指针的所有权被绑定到了一个 FunctionPtr 的对象上，因此相关的大部分实现都只需
要去操纵这个指针即可。代码中未被实现的、也是比较有意思的一部分，将在接下来的章节
中进行介绍。
     */
  
    // primary template:
    template<typename Signature>
    class FunctionPtr;
    
    // partial specialization:
    template<typename R, typename... Args>
    class FunctionPtr<R(Args...)>
    {
      friend bool operator==(FunctionPtr const& f1, FunctionPtr const& f2) {
        if (!f1 || !f2) {
          return !f1 && !f2;
        }
        return f1.bridge->equals(f2.bridge);
      }
      friend bool operator!=(FunctionPtr const& f1, FunctionPtr const& f2) {
        return !(f1 == f2);
      }
    public:
    private:
      FunctorBridge<R, Args...>* bridge;
    public:
      // constructors:
      FunctionPtr() : bridge(nullptr) { }
      
      FunctionPtr(FunctionPtr const& other); // see functionptrcpinv.hpp
      FunctionPtr(FunctionPtr& other) : FunctionPtr(static_cast<FunctionPtr const&>(other)) { }
      
      FunctionPtr(FunctionPtr&& other) : bridge(other.bridge) {
                                                            other.bridge = nullptr;
                                                            }
      
      //construction from arbitrary function objects:
      template<typename F>
      FunctionPtr(F&& f);  // see functionptrinit.hpp
      
      // assignment operators:
      FunctionPtr& operator=(FunctionPtr const& other) {
                                                     FunctionPtr tmp(other);
                                                     swap(*this, tmp);
                                                     return *this;
                                                     }
      
      FunctionPtr& operator=(FunctionPtr&& other) {
                                                delete bridge;
                                                bridge = other.bridge;
                                                other.bridge = nullptr;
                                                return *this;
                                                }
      
      //construction and assignment from arbitrary function objects:
      template<typename F> FunctionPtr& operator=(F&& f) {
                                                       FunctionPtr tmp(std::forward<F>(f));
                                                       swap(*this, tmp);
                                                       return *this;
                                                       }
      
      // destructor:
      ~FunctionPtr() { delete bridge; }
      friend void swap(FunctionPtr& fp1, FunctionPtr& fp2) {
                                                         std::swap(fp1.bridge, fp2.bridge);
                                                         }
      explicit operator bool() const {
        return bridge == nullptr;
      }
      // invocation:
      R operator()(Args... args) const; // see functionptr-cpinv.hpp
    };
  
    template<typename T>
    class IsEqualityComparable
    {
    private:
      // test convertibility of == and ! == to bool:
      static void* conv(bool); // to check convertibility to bool
      template<typename U>
      static std::true_type test(decltype(conv(std::declval<U const&>() == std::declval<U const&>())),
      decltype(conv(!(std::declval<U const&>() == std::declval<U const&>()))));
      
      // fallback:
      template<typename U>
      static std::false_type test(...);
      
    public:
      static constexpr bool value = decltype(test<T>(nullptr, nullptr))::value;
    };
  
    template<typename T, bool EqComparable =
    IsEqualityComparable<T>::value>
    struct TryEquals
    {
      static bool equals(T const& x1, T const& x2) { return x1 == x2; }
    };
    
    class NotEqualityComparable : public std::exception { };
    
    template<typename T>
    struct TryEquals<T, false>
    {
      static bool equals(T const& x1, T const& x2) { throw NotEqualityComparable(); }
    };
    
    template<typename Functor, typename R, typename... Args>
    class SpecificFunctorBridge : public FunctorBridge<R, Args...> {
    private:
      Functor functor;
    public:
      template<typename FunctorFwd>
      SpecificFunctorBridge(FunctorFwd&& functor) : functor(std::forward<FunctorFwd>(functor)) { }
      
      /*
      该实现是正确的，但是不幸的是，它也有一个缺点：如果 FunctionPtr 被一个没有实现合适
的 operator==的函数对象（比如 lambdas）赋值，或者是被这一类对象初始化，那么这个程
序会遇到编译错误。这可能会很让人意外，因为 FunctionPtrs 的 operator==可能根本就没有
被使用，却遇到了编译错误。而诸如 std::vector 之类的模板，只要它们的 operator==没有被
使用，它们就可以被没有相应 operator==的类型实例化。
这一 operator==相关的问题是由类型擦除导致的：因为在给 FunctionPtr 赋值或者初始化的时
候，我们会丢失函数对象的类型信息，因此在赋值或者初始化完成之前，就需要捕捉到所有
所需要知道的该类型的信息。该信息就包含调用函数对象的 operator==所需要的信息，因为
我们并不知道它在什么时候会被用到。
       幸运的是，我们可以使用基于 SFINAE 的萃取技术（见 19.4 节），在调用 operator==之前，
确认它是否可用，如下IsEqualityComparable
       */
  
      virtual bool equals(FunctorBridge<R, Args...> const* fb) const override
      {
        if (auto specFb = dynamic_cast<SpecificFunctorBridge const*> (fb))
          // return functor == specFb->functor;
          return TryEquals<Functor>::equals(functor, specFb->functor);  // 即使所存储的函数对象没有提供operator==,只要不对其进行调用，就不会出错
        //functors with different types are never equal:
        return false;
      }
      
      virtual SpecificFunctorBridge* clone() const override {
        return new SpecificFunctorBridge(functor);
      }
      
      virtual R invoke(Args... args) const override {
        return functor(std::forward<Args>(args)...);
      }
    };
    
    template <typename R, typename ... Args>
    template <typename F>
    FunctionPtr<R(Args...)>::FunctionPtr(F && f) : bridge(nullptr) {
      using Functor = std::decay_t<F>;
      using Bridge = SpecificFunctorBridge<Functor, R, Args...>;
      bridge = new Bridge(std::forward<F>(f));
    }
    
    template<typename R, typename... Args>
    R FunctionPtr<R(Args...)>::operator()(Args... args) const {
      return bridge->invoke(std::forward<Args>(args)...);
    }
  
    template<typename R, typename... Args>
    FunctionPtr<R(Args...)>::FunctionPtr(FunctionPtr const& other) : bridge(nullptr)
    {
      if (other.bridge)
        bridge = other.bridge->clone();
    }
    
    /*
    注意，此处由于 FunctionPtr 的构造函数本身也被函数对象类型模板化了，该类型只为
SpecificFunctorBridge 的特定偏特化版本（以 Bridge 类型别名表述）所知。一旦新开辟的 Bridge
实例被赋值给数据成员 bridge，由于从派生类到基类的转换（Bridge* --> FunctorBridge<R,
Args...>*），特定类型 F 的额外信息将会丢失。类型信息的丢失，解释了为什么名称“类型
擦除”经常被用于描述用来桥接 static 和 dynamic 多态的技术。
该实现的一个特点是在生成 Functor 的类型的时候使用了 std::decay，这使得被推断出来的类
型 F 可以被存储，比如它会将指向函数类型的引用 decay 成函数指针类型，并移除了顶层
const，volatile 和引用。
     */
  
    
    void forUpTo(int n, FunctionPtr<void(int)> f) {
      for (int i = 0; i != n; ++i)
        f(i); // call passed function f for i
    }
  
    void printInt(int i) { std::cout << i << ' '; }
    
    void test()
    {
      std::vector<int> values;
      
      // insert values from 0 to 4:
      forUpTo(5, [&values](int i) { values.push_back(i); });
      
      forUpTo(5, printInt); // prints 0 1 2 3 4
      
      std::cout << '\n';
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
//main_ch22_2()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch22_2::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch22_2::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch22_2::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch22_2::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch22_2::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}
