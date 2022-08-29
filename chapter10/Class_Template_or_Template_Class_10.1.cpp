//
// Created by yangfeng on 2022/7/30.
//

#include "../headers.h"

using namespace std;

/*
在 C++中，structs，classes 以及 unions 都被称为 class types。
 如果没有特殊声明的话，“class” 的字面意思是用关键字 class 或者 struct 声明的 class types。
 注意 class types 包含 unions，但是 class 不包含。
 
关于该如何称呼一个是模板的类，有一些困扰：
   术语 class template 是指这个 class 是模板。也就是说它是一组 class 的参数化表达。
  
   术语 template class 则被：
     用作 class template 的同义词。
     用来指代从 template 实例化出来的 classes。
     用来指代名称是一个 template-id（模板名 + <模板参数>）的类。
     第二种和第三中意思的区别很小，对后续的讨论也不重要。
     由于这一不确定性，在本书中会避免使用术语 template class。
 
同样地，我们会使用 function template，member template，member function template，以及
variable template ， 但 不 会 使 用 template function ， template member ， template member
function，以及 template variable。
 */

namespace ch10_1 {
  
  template <typename T>
  typename std::enable_if_t<(sizeof(T) > 4), T> func() {
  
  }
  
  template <typename T, typename = std::enable_if_t<(sizeof(T) < 3)>>
  void foo() {
    cout << "foo()" << endl;
  }
  
  void test_foo() {
  }
  

  class Tmp {
  public:
  
  };

}

int
//main()
main_Class_Template_or_Template_Class_10_1()
{

  return 0;
}
