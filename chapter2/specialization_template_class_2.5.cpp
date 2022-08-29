//
// Created by yangfeng on 2022/7/16.
//
#include "../headers.h"

using namespace std;

/**
 * 为了特化一个模板类，在类模板声明的前面需要有一个template<>，
 * 并且需要指明所需要特化的类型，这些用于特化类模板的类型被用作模板参数，并且
 * 需要紧跟在类名后面
 */

namespace ch2_5 {

  template<typename T>
  class Stack {
  public:
    void push(T const & element) {
    
    }
  
  };
  
  // 特化
  // 对于被特化的模板，所有成员函数的定义都应该被定义成“常规”成员函数，也就是
  // 说，所有出现T的地方，都应该被替换成用于特化类模板的类型
  template <>
  class Stack<int> {
  public:
    void push(int const & val) {
    
    }
  };

}

int
//main()
main_specialization_template_class()
{

  return 0;
}
