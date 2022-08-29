//
// Created by yangfeng on 2022/7/18.
//
#include "../headers.h"

using namespace std;

/*
 * 从 C++17 开始，可以不指定非类型模板参数的具体类型（代之以 auto），
 *   从而使其可以用于任意有效的非类型模板参数的类型。
 */

namespace ch3_4 {
  
  /*
   * 定义了类型待定的 Maxsize。它的类型可以是任意非类型参数所允许的类型。
   *   在模板内部，既可以使用它的值：
   *   std::array<T,Maxsize> elems; // elements
   *   也可以使用它的类型：
   *   using size_type = decltype(Maxsize);
   */
  template <typename T, auto Maxsize>
  class Stack {
  public:
    using size_type = decltype(Maxsize);
  private:
    std::array<T, Maxsize> elems;
    size_type numElems;
  public:
    Stack();
    void push(T const & elem);
    void pop();
    T const & top() const;
    bool empty() const {
      return numElems == 0;
    }
    
    /*
     * 从 C++14 开始，也可以通过使用 auto，让编译器推断出具体的返回类型：
     *  auto size() const { //return current number of elements
     *      return numElems;
     *  }
     */
    size_type size() const {
      return numElems;
    }
  
  };
  
  template<typename T, auto Maxsize>
  Stack<T, Maxsize>::Stack() : numElems(0) {
  
  }
  
  template<typename T, auto Maxsize>
  void Stack<T, Maxsize>::push(const T &elem) {
    assert(numElems < Maxsize);
    elems[numElems] = elem;
    ++numElems;
  
  }
  
  template<typename T, auto Maxsize>
  void Stack<T, Maxsize>::pop() {
    assert(!elems.empty());
    --numElems;
  }
  
  template<typename T, auto Maxsize>
  T const &Stack<T, Maxsize>::top() const {
    assert(!elems.empty());
    return elems[numElems - 1];
  }
  
  
  class Tmp {
  public:
  
  };
  
  template <auto T>
  class Message {
  public:
    void print() {
      std::cout << T << '\n';
    }
  
  };
  
  /*
   * 可以使用template< decltype(auto) N>，将N实例化成引用类型
   */
  template <decltype(auto) N>
  class C {
  public:
    void print() {
      cout << N << endl;
    }
  
  };
  
  class Test {
    friend ostream& operator<<(ostream&, const Test &);
  public:
    static int copy_cnt;
  public:
    Test() { }
    Test(const Test & t) {
      ++copy_cnt;
      cout << "copy ctor" << endl;
    }
    
  };
  ostream& operator<<(ostream& strm, const Test& t) {
    strm << "copy_cnt: " << t.copy_cnt << endl;
    return strm;
  }
  
  int Test::copy_cnt = 0;
  
  void test_message() {
    ch3_4::Message<43> m1;
    m1.print();
    static char const str[] = "hello, world";
    ch3_4::Message<str> m2;
    m2.print();
    
  }

}

int
//main()
main_template_parameter_type_auto()
{
  ch3_4::Test t1;
  

  return 0;
}
