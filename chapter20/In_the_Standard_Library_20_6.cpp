//
// Created by yangfeng on 2022/8/25.
//

#include "../headers.h"

using namespace std;

/*
20.6 在标准库中的情况
C++标准库为输入，输出，前向，双向以及随机访问迭代器提供了迭代器标记，我们对这些
都已经做了展示。这些迭代器标记是标准迭代器萃取（std::iterator
_traits）技术以及施加于迭代器的需求的一部分，因此它们可以被安全得用于标记派发。
C++11 标准库中的 std::enable_if 模板提供了和我们所展示的 EnableIf 相同的行为。唯一的不
同是标准库用了一个小写的成员类型 type，而我们使用的是 Type。
算法的偏特化在 C++标准库中被用在了很多地方。比如，std::advance()以及 std::distance()基
于其迭代器参数的种类的不同，都有很多变体。虽然很多标准库的实现都倾向于使用标记派
发（tag dispatch），但是最近其中一些实现也已经使用 std::enable_if 来进行算法特化了。而
且，很多的 C++标准库的实现，在内部也都用这些技术去实现各种标准库算法的偏特化。比
如，当迭代器指向连续内存且它们所指向的值有拷贝赋值运算符的时候，std::copy()可以通
过调用 std::memory()和 std::memmove()来进行偏特化。同样的，std::fill()也可以通过调用
std::memset 进行优化，而且在知晓一个类型有一个普通的析构函数（trivial destructor）的
情况下，很多算法都可以避免去调用析构函数。C++标准并没有对这些算法特化的实现方式
进行统一（比如统一采用 std::advance()和 std::distance()的方式），但是实现者还是为了性能
而选择类似的方式。
正如第 8.4 节介绍的那样，C++标准库强烈的建议在其所需要施加的条件中使用 std::enable_if
<>或者其它类似 SFINAE 的技术。比如，std::vector 就有一个允许其从迭代器序列进行构造的
构造函数模板：
template<typename InputIterator>
vector(InputIterator first, InputIterator second,
allocator_type const& alloc = allocator_type());
它要求“当通过类型 InputIterator 调用构造函数的时候，如果该类型不属于输入迭代器（input
iterator），那么该构造函数就不能参与到重载解析中”（参见第 23.2.3 节）。这一措辞并
没有精确到足以使当前最高效的技术被应用到实现当中，但是在其被引入到标准中的时候，
std::enable_if<>确实被寄予了这一期望。
 */

namespace ch20_6 {

  namespace case1 {
    void test() {
    
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
main_ch20_6()
{
  ch20_6::case1::test();

  return 0;
}