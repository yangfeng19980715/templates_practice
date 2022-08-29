//
// Created by yangfeng on 2022/7/20.
//

#include "../headers.h"

using namespace std;

/*
 从 C++14 开始，变量也可以被某种类型参数化。称为变量模板。
  例如可以通过下面的代码定义 pi，但是参数化了其类型：
    template<typename T>
    constexpr T pi{3.1415926535897932385};
  注意，和其它几种模板类似，这个定义最好不要出现在函数内部或者块作用域内部。
  
  在使用变量模板的时候，必须指明它的类型。
  比如下面的代码在定义 pi<>的作用域内使用了两个不同的变量：
    std::cout << pi<double> << ’\n’;
    std::cout << pi<float> << ’\n’;
 */

namespace ch5_6 {
  
  /*
   * 变量模板也可以用于不同编译单元。
   * 也可有默认模板类型。
   * 只是无论怎样都要使用尖括号<>。
   *
   * 同样可以用非类型参数对变量模板进行参数化，也可以将非类型参数用于参数器的初始化。
  template<int N>
  std::array<int,N> arr{};      // array with N elements, zero-initialized
  
  template<auto N>
  constexpr decltype(N) dval = N;   // type of dval depends on passed value
  
  int main()
  {
    std::cout << dval<’c’> << ’\n’; // N has value ’c’ of type char
    arr<10>[0] = 42;        // sets first element of global arr
    for (std::size_t i=0; i<arr<10>.size(); ++i) {   // uses values set in arr
      std::cout << arr<10>[i] << ’\n’;
    }
  }
   */
  
  template <typename T>
  constexpr T pi{3.1415926535897932385};
  
  class TestPi {
  public:
    void func() {
      std::cout << pi<double> << std::endl;
    }
  };
  
  /*
  用于数据成员的变量模板
  变量模板的一种应用场景是，用于定义代表类模板成员的变量模板。
   比如如果像下面这样定义一个类模板：
    template<typename T>
    class MyClass {
    public:
      static constexpr int max = 1000;
    };
    
    那么就可以为 MyClass<>的不同特例化版本定义不同的值：
      template<typename T>
      int myMax = MyClass<T>::max;
    
    应用工程师就可以使用下面这样的代码：
      auto i = myMax<std::string>;
    而不是：
      auto i = MyClass<std::string>::max;
    这意味着对于一个标准库的类：
      namespace std {
        template<typename T>
        class numeric_limits {
        public:
          …
          static constexpr bool is_signed = false;
          …
        };
      }
    可以定义：
      template<typename T>
      constexpr bool isSigned = std::numeric_limits<T>::is_signed;
      
    这样就可以用：
      isSigned<char>
    代替：
      std::numeric_limits<char>::is_signed
   */

  /*
  类型萃取 Suffix_v
  从 C++17 开始，标准库用变量模板为其用来产生一个值（布尔型）的类型萃取定义了简化方式。
   比如为了能够使用：
    std::is_const_v<T> // since C++17
  而不是：
    std::is_const<T>::value //since C++11
  标准库做了如下定义：
    namespace std {
      template<typename T>
      constexpr bool is_const_v = is_const<T>::value;
    }
   */
  
  class Tmp {
  public:
  
  };

}

int
//main()
main_variable_templates()
{
  ch5_6::TestPi tp { };
  tp.func();

  return 0;
}