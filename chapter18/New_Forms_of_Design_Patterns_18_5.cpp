//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

using namespace std;

/*
新形势的设计模式
  C++中的 static 多态给经典的设计模式提供了新的实现方式。
    以桥接模式为例（bridge pattern，它在很多 C++程序中扮演了重要的角色）。
    使用桥接模式的一个目的是在不同的接口实现之间做切换。
    
  根据[DesignPatternsGoF]，桥接模式通常是通过使用一个接口类实现的，
    在这个接口类中包含了一个指向具体实现的指针，然后通过该指针委派所有的函数调用（参见图 18.3）。
    
  但是，如果具体实现的类型在编译期间可知，我们也可以利用模板实现桥接模式（参见图18.4）。
    这样做会更类型安全（一部分原因是避免了指针转换），而且性能也会更好。
 */

namespace ch18_5 {

  namespace case1 {
  }
  
  namespace case2 {
  }
  
  namespace case3 {
  }
  
  namespace case4 {
  }
  
  namespace case5 {
  
  }

  class Tmp {
  public:
  
  };

}

int
//main()
main_New_Forms_of_Design_Patterns_18_5()
{

  return 0;
}