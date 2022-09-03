//
// Created by yangfeng on 9/3/2022.
//

#include "../headers.h"

using namespace std;

/*
 22.5 可选桥接（Optional Bridging）
上述 FunctionPtr 实现几乎可以被当作一个函数指针的非正式替代品适用。但是它并没有提
供对下面这一函数指针操作的支持：检测两个 FunctionPtr 的对象是否会调用相同的函数。
为了实现这一功能，需要在 FunctorBridge 中加入 equals 操作：
virtual bool equals(FunctorBridge const* fb) const = 0;
在 SpecificFunctorBridge 中的具体实现如下：
virtual bool equals(FunctorBridge<R, Args…> const* fb) const override
{
if (auto specFb = dynamic_cast<SpecificFunctorBridge const*> (fb))
{
return functor == specFb->functor;
}
五车书馆
300
//functors with different types are never equal:
return false;
}
最后可以为 FunctionPtr 实现 operator==，它会先检查对应内容是否是 null，然后将比较委托
给 FunctorBridge:
friend bool
operator==(FunctionPtr const& f1, FunctionPtr const& f2) {
if (!f1 || !f2) {
return !f1 && !f2;
}
return f1.bridge->equals(f2.bridge);
}
friend bool
operator!=(FunctionPtr const& f1, FunctionPtr const& f2) {
return !(f1 == f2);
}
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
确认它是否可用，如下：
#include <utility> // for declval()
#include <type_traits> // for true_type and false_type
template<typename T>
class IsEqualityComparable
{
private:
// test convertibility of == and ! == to bool:
static void* conv(bool); // to check convertibility to bool
template<typename U>
static std::true_type test(decltype(conv(std::declval<U
const&>() == std::declval<U const&>())),
五车书馆
301
decltype(conv(!(std::declval<U const&>() == std::declval<U
const&>()))));
// fallback:
template<typename U>
static std::false_type test(…);
public:
static constexpr bool value = decltype(test<T>(nullptr,
nullptr))::value;
};
上述 IsEqualityComparable 技术使用了在 19.4.1 节介绍的表达式测试萃取的典型形式：两个
test()重载，其中一个包含了被封装在 decltype 中的用来测试的表达式，另一个通过省略号
接受任意数量的参数。第一个 test()试图通过==去比较两个 T const 类型的对象，然后确保两
个结果都可以被隐式的转换成 bool，并将可以转换为 bool 的结果传递给 operator!=()。如果
两个运算符都正常的话，参数类型都将是 void *。
使用 IsEqualityComparable，可以构建一个 TryEquals 类模板，它要么会调用==运算符（如果
可用的话），要么就在没有可用的 operator==的时候抛出一个异常：
#include <exception>
#include "isequalitycomparable.hpp"
template<typename T, bool EqComparable =
IsEqualityComparable<T>::value>
struct TryEquals
{
static bool equals(T const& x1, T const& x2) {
return x1 == x2;
}
};
class NotEqualityComparable : public std::exception
{ };
template<typename T>
struct TryEquals<T, false>
{
static bool equals(T const& x1, T const& x2) {
throw NotEqualityComparable();
}
}
最后，通过在 SpecificFunctorBridge 中使用 TryEquals，当被存储的函数对象类型一致，而且
支持 operator==的时候，就可以在 FunctionPtr 中提供对 operator==的支持：
virtual bool equals(FunctorBridge<R, Args…> const* fb) const override
五车书馆
302
{
if (auto specFb = dynamic_cast<SpecificFunctorBridge const*>(fb)) {
return TryEquals<Functor>::equals(functor, specFb->functor);
}
//functors with different types are never equal:
return false;
}
 */

namespace ch22_5 {

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
main_ch22_5()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch22_5::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch22_5::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch22_5::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch22_5::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch22_5::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}