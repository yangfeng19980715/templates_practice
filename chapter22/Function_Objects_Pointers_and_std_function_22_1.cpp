//
// Created by yangfeng on 9/1/2022.
//

#include "../headers.h"

using namespace std;

/*
第 22 章 桥接 static 和 dynamic 多态
在第 18 章中介绍了 C++中 static 多态（通过模板实现）和 dynamic 多态（通过继承和 virtual
函数实现）的本质。两种多态都给程序编写提供了功能强大的抽象，但是也都各有其不足：
static 多态提供了和非多态代码一样的性能，但是其在运行期间所使用的类型在编译期就已
经决定。而通过继承实现的 dynamic 多态，则允许单一版本的多态函数适用于在编译期未知
的类型，但是该方式有点不太灵活，因为相关类型必须从统一的基类做继承。
本章将介绍在 C++中把 static 多态和 dynamic 多态桥接起来的方式，该方式具备了在第 18.3
节中介绍的各种模型的部分优点：比较小的可执行代码量，几乎全部的动态多态的编译期特
性，以及（允许内置类型无缝工作的）静态多态的灵活接口。作为例子，我们将创建一个简
化版的 std::function<>模板。
22.1 函数对象，指针，以及 std:function<>
在给模板提供定制化行为的时候，函数对象会比较有用。比如，下面的函数模板列举了从 0
到某个值之间的所有整数，并将每一个值都提供给了一个已有的函数对象 f：
#include <vector>
#include <iostream>
template<typename F>
void forUpTo(int n, F f){
for (int i = 0; i != n; ++i)
{
f(i); // call passed function f for i
}
}
void printInt(int i)
{
std::cout << i << ’ ’;
}
int main()
{
std::vector<int> values;
// insert values from 0 to 4:
forUpTo(5,
[&values](int i) {
values.push_back(i);
});
五车书馆
294
// print elements:
forUpTo(5, printInt); // prints 0 1 2 3 4
std::cout << ’\n’;
}
其中 forUpTo()函数模板适用于所有的函数对象，包括 lambda，函数指针，以及任意实现了
合适的 operator()运算符或者可以转换为一个函数指针或引用的类，而且每一次对 forUpTo()
的使用都很可能产生一个不同的函数模板实例。上述例子中的函数模板非常小，但是如果该
模板非常大的话，这些不同应用导致的实例化很可能会导致代码量的增加。
一个缓解代码量增加的方式是将函数模板转变为非模板的形式，这样就不再需要实例化。比
如，我们可能会使用函数指针：
void forUpTo(int n, void (*f)(int))
{
for (int i = 0; i != n; ++i)
{
f(i); // call passed function f for i
}
}
但是，虽然在给其传递 printInt()的时候该方式可以正常工作，给其传递 lambda 却会导致错
误：
forUpTo(5,
printInt); //OK: prints 0 1 2 3 4
forUpTo(5,
[&values](int i) { //ERROR: lambda not convertible to a function
pointer
values.push_back(i);
});
标准库中的类模板 std::functional<>则可以用来实现另一种类型的 forUpTo():
#include <functional>
void forUpTo(int n, std::function<void(int)> f)
{
for (int i = 0; i != n; ++i)
{
f(i) // call passed function f for i
}
}
Std::functional<>的模板参数是一个函数类型，该类型体现了函数对象所接受的参数类型以及
其所需要产生的返回类型，非常类似于表征了参数和返回类型的函数指针。
五车书馆
295
这一形式的 forUpTo()提供了 static 多态的一部分特性：适用于一组任意数量的类型（包含函
数指针，lambda，以及任意实现了适当 operator()运算符的类），同时又是一个只有一种实
现的非模板函数。为了实现上述功能，它使用了一种称之为类型消除（type erasure）的技
术，该技术将 static 和 dynamic 多态桥接了起来。
 */

namespace ch22_1 {

  namespace case1 {
    
    template<typename F>
    void forUpTo(int n, F f) {
      
      for (int i = 0; i != n; ++i)
        f(i); // call passed function f for i
    }
    
    void printInt(int i) {  std::cout << i << endl; }
    
    void test() {
      std::vector<int> values;
      
      // insert values from 0 to 4:
      forUpTo(5, [&values](int i) { values.push_back(i); } );
      
      // print elements:
      forUpTo(5, printInt); // prints 0 1 2 3 4
      
      std::cout << endl;
    }
    
    /*
     其中 forUpTo()函数模板适用于所有的函数对象，包括 lambda，函数指针，以及任意实现了
合适的 operator()运算符或者可以转换为一个函数指针或引用的类，而且每一次对 forUpTo()
的使用都很可能产生一个不同的函数模板实例。上述例子中的函数模板非常小，但是如果该
模板非常大的话，这些不同应用导致的实例化很可能会导致代码量的增加。
一个缓解代码量增加的方式是将函数模板转变为非模板的形式，这样就不再需要实例化。比
如，我们可能会使用函数指针：
     */
    void forUpTo2(int n, void (*f)(int)) {
      for (int i = 0; i != n; ++i)
        f(i); // call passed function f for i
    }
  
    // 但是，虽然在给其传递 printInt()的时候该方式可以正常工作，给其传递 lambda 却会导致错误：
    // 标准库中的类模板 std::functional<>则可以用来实现另一种类型的 forUpTo():
    
    void forUpTo(int n, std::function<void(int)> f) {
      for (int i = 0; i != n; ++i)
        f(i); // call passed function f for i
    }
    
    /*
    Std::functional<>的模板参数是一个函数类型，该类型体现了函数对象所接受的参数类型以及
其所需要产生的返回类型，非常类似于表征了参数和返回类型的函数指针。
这一形式的 forUpTo()提供了 static 多态的一部分特性：适用于一组任意数量的类型（包含函
数指针，lambda，以及任意实现了适当 operator()运算符的类），同时又是一个只有一种实
现的非模板函数。为了实现上述功能，它使用了一种称之为类型消除（type erasure）的技
术，该技术将 static 和 dynamic 多态桥接了起来。
     */
    
    
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
// main()
main_ch22_1()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch22_1::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch22_1::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch22_1::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch22_1::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch22_1::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}