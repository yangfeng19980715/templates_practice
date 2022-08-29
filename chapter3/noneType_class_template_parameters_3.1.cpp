//
// Created by yangfeng on 2022/7/18.
//
#include "../headers.h"

using namespace std;

/*
 * 对于之前介绍的函数模板和类模板，其模板参数不一定非得是某种具体的类型，
 *   也可以是常规数值。
 *
 *   在使用这种模板时，需要显式地指出待定数值地具体值，之后代码会被实例化。
 *
 *
 */

namespace ch3_1 {
  
  /*
   * 可以定义一个使用固定尺寸的array作为容器的stack，这个尺寸的大小最好由stack的用户自己来指定。
   *
   */

  
  // 对于非类型模板参数，也可以指定默认值
  // 如：
  // template <typename T, std::size_t Maxsize = 100>
  template <typename T, std::size_t Maxsize>
  class Stack {
  private:
    std::array<T, Maxsize> elems;
    std::size_t numElems;
  public:
    Stack();
    void push(T const & elem);
    void pop();
    T const & top() const;
    bool empty() const {
      return numElems == 0;
    }
    std::size_t size() const {
      return numElems;
    }
  
  };
  
  template<typename T, std::size_t Maxsize>
  Stack<T, Maxsize>::Stack() : numElems(0){ // start with no elements
  }
  
  template<typename T, std::size_t Maxsize>
  void Stack<T, Maxsize>::push(const T &elem) {
    //static_assert(numElems < Maxsize, "Stack is full");
    assert(numElems < Maxsize);
    elems[numElems] = elem; // append element
    ++numElems; // increment number of elements
  }
  
  template<typename T, std::size_t Maxsize>
  void Stack<T, Maxsize>::pop() {
    //static_assert(!elems.empty(), "Stack is empty");
    assert(!elems.empty());
    --numElems;
  }
  
  template<typename T, std::size_t Maxsize>
  T const &Stack<T, Maxsize>::top() const {
    //static_assert(!elems.empty(), "Stack is empty");
    assert(!elems.empty());
    return elems[numElems - 1];
  }
  
}

int
//main()
main_noneType_class_template_parameters()
{
  ch3_1::Stack<int, 10>  stk;
  for (int i = 0; i < 10; ++i)
    stk.push(i + 10);
  
  cout << "stack.size(): " << stk.size() << endl;
  cout << "top: " << stk.top() << endl;
  
  
  cout << "stack.size(): " << stk.size() << endl;

  return 0;
}
