//
// Created by yangfeng on 9/20/2022.
//

#include "../headers.h"

using namespace std;

/*
 25.2 基础元组操作
25.2.1 比较
元组是包含了其它数值的结构化类型。为了比较两个元组，就需要比较它们的元素。因此可
以像下面这样，定义一种能够逐个比较两个元组中元素的 operator==：
// basis case:
bool operator==(Tuple<> const&, Tuple<> const&)
{
// empty tuples are always equivalentreturn true;
}
// recursive case:
template<typename Head1, typename… Tail1,
typename Head2, typename… Tail2,
typename = std::enable_if_t<sizeof…(Tail1)==sizeof…(Tail2)>>
bool operator==(Tuple<Head1, Tail1…> const& lhs, Tuple<Head2, Tail2…> const& rhs)
{
return lhs.getHead() == rhs.getHead() &&
lhs.getTail() == rhs.getTail();
}
（应该还需要定义一版 sizeof…(Tail1) != sizeof…(Tail2)的 operator==）
和其它适用于类型列表和元组的算法类似，逐元素的比较两个元组，会先比较首元素，然后
递归地比较剩余的元素，最终会调用 operator 的基本情况结束递归。运算符!=，<，>，以及>=
的实现方式都与之类似。
五车书馆
347
25.2.2 输出
贯穿本章始终，我们一直都在创建新的元组类型，因此最好能够在执行程序的时候看到这些
元组。下面的 operator<<运算符会打印那些元素类型可以被打印的元组：
#include <iostream>
void printTuple(std::ostream& strm, Tuple<> const&, bool isFirst = true)
{
strm << ( isFirst ? ’(’ : ’)’ );
}
template<typename Head, typename… Tail>
void printTuple(std::ostream& strm, Tuple<Head, Tail…> const& t, bool isFirst =
true)
{
strm << ( isFirst ? "(" : ", " );
strm << t.getHead();
printTuple(strm, t.getTail(), false);
}
template<typename … Types>
std::ostream& operator<<(std::ostream& strm, Tuple<Types…> const& t)
{
printTuple(strm, t);
return strm;
}
这样就可以很容易地创建并打印元组了。比如：
std::cout << makeTuple(1, 2.5, std::string("hello")) << ’\n’;
会打印出：
(1, 2.5, hello)
 */

namespace ch25_2 {

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
main_ch25_2()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch25_2::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch25_2::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch25_2::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch25_2::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch25_2::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}