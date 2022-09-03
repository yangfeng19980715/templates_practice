//
// Created by yangfeng on 9/2/2022.
//

#include "../headers.h"

using namespace std;

/*
 22.3 桥接接口（Bridge Interface）
FunctorBridge 类模板负责持有以及维护底层的函数对象，它被实现为一个抽象基类，为
FunctionPtr 的动态多态打下基础：
template<typename R, typename… Args>
class FunctorBridge
{
public:
五车书馆
298
virtual ~FunctorBridge() {
}
virtual FunctorBridge* clone() const = 0;
virtual R invoke(Args… args) const = 0;
};
FunctorBridge 通过虚函数提供了用来操作被存储函数对象的必要操作：一个析构函数，一个
用来执行 copy 的 clone()操作，以及一个用来调用底层函数对象的 invoke()操作。不要忘记将
clone()和 invoke()声明为 const 的成员函数。
有了这些虚函数，就可以继续实现 FunctionPtr 的拷贝构造函数和函数调用运算符了：
template<typename R, typename… Args>
FunctionPtr<R(Args…)>::FunctionPtr(FunctionPtr const& other)
: bridge(nullptr)
{
if (other.bridge) {
bridge = other.bridge->clone();
}
}
template<typename R, typename… Args>
R FunctionPtr<R(Args…)>::operator()(Args&&… args) const
{
return bridge->invoke(std::forward<Args>(args)…);
}
 */

namespace ch22_3 {

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
main_ch22_3()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch22_3::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch22_3::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch22_3::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch22_3::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch22_3::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}