//
// Created by yangfeng on 2022/7/16.
//
#include "../headers.h"

using namespace std;

/**
 * 通过给类模板定义一个新名字，可以使类模板的使用变得更方便
 *
 * 为了简化给类模板定义新名字的过程，有两种方法可用：
 *   1. 使用关键字typedef
 *   typedef stack<int> IntStack;
 *   void foo(IntStack const & s);
 *   称这种声明方式为typedef， 被定义的名字叫做typedef-name.
 *
 *   2. 使用关键字using（从c++11开始）
 *   using IntStack = stack<int>
 *   void foo(IntStack const & s);
 *
 *   这一过程叫做alias declaration。
 *   以上两种给一个已经存在的类型定义新名字的方式，被称为type alias declaration。新
 *   的名字被称为type alias。
 */
 
/**
 *  别名模板
 *
 *  不同于typedef， alias declaration也可以被模板化，这样就可以给一组类型取一个方便的名字
 *  这一特性从c++11开始生效，被称作alias templates。
 *  如：
 *  template <typename T>
 *  using DequeStack = Stack<T, std::deque<T>>
 */
 

/**
 *  class 成员的别名模板
 *  使用alias template 可以很方便地给类模板的成员定义一个快捷方式
 *  如：
 *   template<typename T>
 *   struct MyType {
 *     using iterator = ...;
 *   };
 *
 *   template<typename T>
 *   using MyTypeIterator = typename MyType<T>::iterator;
 *
 *   允许我们使用
 *     MyTypeIterator<int> pos;
 *   取代
 *     typename MyType<T>::iterator pos;
 *
 *
 */
 

/**
 *  suffix_t类型萃取(type traits suffix_t)
 *  从c++14开始，标准库使用上面的技术，给标准库中所有返回一个类型的type trait定义了
 *  快捷方式。
 *  比如，为了能够使用：
 *  std::add_const_t<T>   // since c++14
 *  而不是：
 *  typename std::add_const<T>::type // since c++11
 *
 *  标准库做了如下定义：
 *  namespace std {
 *    template <typename T>
 *    using add_const_t = typename std::add_const<T>::type;
 *  }
 *
 */

namespace ch2_8 {

  class Tmp {
  public:
  
  };

}

int
//main()
main_type_alias()
{
  using const_int = std::add_const_t<int>;
  const_int a = 10;
  //a = 11;
  
  cout << std::is_volatile_v<decltype(a)> << endl;
  cout << std::is_same_v<decltype(a), const int> << endl;
  cout << std::is_same_v<decltype(a), int const> << endl;

  return 0;
}
