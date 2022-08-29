//
// Created by yangfeng on 2022/7/12.
//
#include "../headers.h"

using namespace std;

namespace ch1 {
  
  class Multi_args_of_template {
  public:
    
    /*
     这里返回值和参数的传入顺序有关,解决办法：
      1. 引入第三个模板参数作为返回类型
      2. 让编译器找出返回类型
      3. 将返回类型定义为两个参数类型的公共类型
     */
    template<typename T1, typename T2>
    T1 max_first(T1 a, T2 b) {
      return a < b ? b : a;
    }
    
    // 1. 作为返回类型的模板参数
    template <typename T1, typename T2, typename RT>
    RT max_second(T1 a, T2 b) {
      return a < b ? b : a;
    }
    template <typename RT, typename T1, typename T2> // 这样的顺序可以只指定第一个模板参数
    RT max_second_2(T1 a, T2 b) { // 调用: max<double>(4, 7.2);
      return a < b ? b : a;
    }
    
    // 2. 返回类型推断
    template<typename T1, typename T2>
    auto max_third(T1 a, T2 b) {  // 从c++14开始
      return a < b ? b : a;
    }
    template<typename T1, typename T2>
    auto max_third_2(T1 a, T2 b) -> decltype(a < b ? b : a) {  // 在c++11中这样做
      return a < b ? b : a;
    }
    // 由于T可能是引用类型，因此返回类型也可能被推断为引用类型，因此应该返回的是decay后的T：
    template<typename T1, typename T2>
    auto max_third_3(T1 a, T2 b) -> typename std::decay<decltype(a < b ? b : a)>::type {
      // 这里使用了类型萃取（type trait）std::decay<>, 它返回其type成员作为目标类型，由于其
      //     type成员是一个类型，为了获取其结果，需要用关键字typename修饰这个表达式
      // 注意：在初始化auto变量的时候，其类型总是退化了之后的类型，auto是返回类型时也是这样
      int i = 42;
      int const & ir = i;
      auto at = ir; // at的类型时ir decay之后的类型，也就是int
      return a < b ? b : a;
    }
    
    // 1.3 将返回类型声明为公共类型
    //   从c++11开始，标准库提供了一种指定“更一般类型”的方式， std::common_type<>::type产生的类型
    //     是他的两个模板参数的公共类型
    template <typename T1, typename T2>
    std::common_type_t<T1, T2> max_fourth(T1 a, T2 b) { // since c++14
      return b < a ? a : b;
    }
    template <typename T1, typename T2>
    typename std::common_type<T1, T2>::type max_fourth_2(T1 a, T2 b) { // since c++11
      return b < a ? a : b;
    }
    
    
    
  };
  
  class Test {
  public:
    
    int m_val;
    Test(int _val = 0) : m_val(_val) { }
    
    //Test(const Test & t) = delete;
    Test(const Test & t) { cout << "copy constructor" << endl; this->m_val = t.m_val; }
    
    bool operator < (const Test& t) const {
      return this->m_val < t.m_val;
    }
    
    
  };
  
  template<typename T>
  T m_max(T& a, T& b) {
    return a < b ? b : a;
  }
  
  template<typename T>
  T void_as_arg(T* t_ptr) {
    if (t_ptr) {
      cout << "not nullptr" << endl;
    }
    else {
      cout << "nullptr" << endl;
    }
  }
  
  void test_default_args(int = 5) {
    cout << __func__ << endl;
  }
  
  template <typename T = std::string>
  void template_default_args(T = "") {
    cout << "function: " << __func__  << "()"<< endl;
  }
  
}

void test_code() {
  ch1::Test t1{ 1 }, t2{ 2 };
  ch1::Test&& tmp = ch1::m_max(t1, t2);
  cout << tmp.m_val << endl;
  void* vp = nullptr;
  ch1::void_as_arg(vp);  // 模板参数被推断为void
  
}

int
main_ch_1_function_templates()
//main()
{
  ch1::template_default_args();
  
  return 0;
}
