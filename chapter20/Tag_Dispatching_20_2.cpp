//
// Created by yangfeng on 2022/8/12.
//

#include "../headers.h"

/*
20.2 标记派发（Tag Dispatching）
  算法特化的一个方式是，用一个唯一的、可以区分特定变体的类型来标记（tag）不同算法
  变体的实现。比如为了解决上述 advanceIter()中的问题，可以用标准库中的迭代器种类标记
  类型，来区分 advanceIter()算法的两个变体实现：
  template<typename Iterator, typename Distance>
  void advanceIterImpl(Iterator& x, Distance n,
  std::input_iterator_tag)
  {
  while (n > 0) { //linear time
  ++x;
  --n;
  }
  }
  template<typename Iterator, typename Distance>
  void advanceIterImpl(Iterator& x, Distance n,
  std::random_access_iterator_tag)
  {
  x += n; // constant time
  }
  然后，通过 advanceIter()函数模板将其参数连同与之对应的 tag 一起转发出去：
  template<typename Iterator, typename Distance>
  void advanceIter(Iterator& x, Distance n)
  {
  advanceIterImpl(x, n, typename
  std::iterator_traits<Iterator>::iterator_category())
  }
  萃取类模板 std::iterator_traits 通过其成员类型 iterator_category 返回了迭代器的种类。迭代
  器种类是前述_tag 类型中的一种，它指明了相关类型的具体迭代器种类。在 C++标准库中，
  可用的 tags 被定义成了下面这样，在其中使用了继承来反映出一个用 tag 表述的种类是不是
  从另一个种类派生出来的：
  namespace std {
  struct input_iterator_tag { };
  struct output_iterator_tag { };
  struct forward_iterator_tag : public input_iterator_tag { };
  五车书馆
  248
  struct bidirectional_iterator_tag : public forward_iterator_tag
  { };
  struct random_access_iterator_tag : public
  bidirectional_iterator_tag { };
  }
  有效使用标记派发（tag dispatching）的关键在于理解 tags 之间的内在关系。我们用来标记
  两个 advanceIterImpl 变体的标记是 std::input_iterator_tag 和 std::random_access_iterator_tag，
  而由于 std::random_access_iterator_tag 继承自 std::input_iterator_tag，对于随机访问迭代器，
  会优先选择更为特化的 advanceIterImpl()变体（使用了 std::random_access_iterator_tag 的那
  一个）。因此，标记派发依赖于将单一的主函数模板的功能委托给一组_impl 变体，这些变
  体都被进行了标记，因此正常的函数重载机制会选择适用于特定模板参数的最为特化的版
  本。
  当被算法用到的特性具有天然的层次结构，并且存在一组为这些标记提供了值的萃取机制的
  时候，标记派发可以很好的工作。而如果算法特化依赖于专有（ad hoc）类型属性的话（比
  如依赖于类型 T 是否含有拷贝赋值运算符），标记派发就没那么方便了。对于这种情况，我
  们需要一个更强大的技术。
 */

using namespace std;

namespace ch20_2 {
  /*
  算法特化的一个方式是，用一个唯一的、可以区分特定变体的类型来标记（tag）不同算法
  变体的实现。比如为了解决上述 advanceIter()中的问题，可以用标准库中的迭代器种类标记
  类型，来区分 advanceIter()算法的两个变体实现：
   */

  namespace case1 {
    template<typename Iterator, typename Distance>
    void advanceIterImpl(Iterator& x, Distance n, std::input_iterator_tag)
    {
      while (n > 0) {         //linear time
        ++x;
        --n;
      }
    }
    template<typename Iterator, typename Distance>
    void advanceIterImpl(Iterator& x, Distance n, std::random_access_iterator_tag)
    {
      x += n;             // constant time
    }
    
    // 然后，通过 advanceIter()函数模板将其参数连同与之对应的 tag 一起转发出去：
    template<typename Iterator, typename Distance>
    void advanceIter(Iterator& x, Distance n)
    {
      advanceIterImpl(x, n, typename std::iterator_traits<Iterator>::iterator_category());
    }
  }
  
  namespace case2 {
  }
  
  namespace case3 {
  }
  
  namespace case4 {
  }
  
  namespace case5 {
  
  }

  class Tmp {
  public:
  
  };

}

int
//main()
main_Tag_Dispatching_20_2()
{

  return 0;
}