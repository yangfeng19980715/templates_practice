//
// Created by yangfeng on 2022/7/20.
//
#include "../headers.h"

using namespace std;

/*
  对于类模板，如果它的基类也是依赖于模板参数的，那么对它而言即使 x 是继承而来的，
  使用 this->x 和 x 也不一定是等效的。
  比如：
  template<typename T>
  class Base {
  public:
    void bar();
  };
  
  template<typename T>
  class Derived : Base<T> {
  public:
    void foo() {
      bar(); // calls external bar() or error
    }
  };
  Derived 中的 bar()永远不会被解析成 Base 中的 bar()。因此这样做要么会遇到错误，要么就
  是调用了其它地方的 bar()（比如可能是定义在其它地方的 global 的 bar()）。
  
  目前作为经验法则，建议当使用定义于基类中的、依赖于模板参数的成员时，
    用 this->或者 Base<T>::来修饰它。
 */

namespace ch5_3 {

  class Tmp {
  public:
  
  };

}

int
//main()
main_using_this_operator_arrow()
{

  return 0;
}
