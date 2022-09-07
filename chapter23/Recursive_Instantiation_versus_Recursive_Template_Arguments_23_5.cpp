//
// Created by yangfeng on 9/7/2022.
//

#include "../headers.h"

using namespace std;

/*
23.5 递归实例化和递归模板参数
考虑如下递归模板：
template<typename T, typename U>
struct Doublify {
};
template<int N>
struct Trouble {
using LongType = Doublify<typename Trouble<N-1>::LongType,
typename Trouble<N-1>::LongType>;
};
template<>
struct Trouble<0> {
using LongType = double;
};
五车书馆
315
Trouble<10>::LongType ouch;
Trouble<10>::LongType 的使用并不是简单地触发形如 Trouble<9>, Trouble<8>, …,
Trouble<0>地递归实例化，还会用越来越复杂地类型实例化 Doublify。表 23.1 展示了其快
速地增长方式：
类型别名
底层类型
Trouble<0>::LongType
double
Trouble<1>::LongType
Doublify<double,double>
Trouble<2>::LongType
Doublify<Doublify<double,double>,
Doublify<double,double>>
Trouble<3>::LongType
Doublify<Doublify<Doublify<double,double>,
Doublify<double,double>>,
<Doublify<double,double>,
Doublify<double,double>>>
表 23.1 Trouble<3>::LongType 的变化趋势
就如从表 23.1 中看到的那样，Trouble<N>::LongType 类型的复杂度与 N 成指数关系。通
常这种情况给 C++编译器带来的压力要远比有递归实例化但是没有递归模板参数的情况要
大。这里的问题之一是，编译器会用一些支离破碎的名字来表达这些类型。这些支离破碎的
名字会用相同的方式去编码模板的特例化，在早期 C++中，这一编码方式的实现和模板签名
（ template-id ） 的 长 度 成 正 比 。 这 些 编 译 器 会 使 用 大 于 10,000 个 字 符 来 表 达
Trouble<N>::LongType。
基于嵌套模板在现在 C++中非常常见的事实，新的 C++实现使用了一种聪明的压缩技术来大
大降低名称编码（比如对于 Trouble<N>::LongType，只需要用数百个字符）的增长速度。
如果没有为某些模板实例生成低层级的代码，那么相关类型的名字就是不需要的，新的编译
器就不会为这些类型产生名字。除此之外，其它情况都没有改善，因此在组织递归实例化代
码的时候，最好不要让模板参数也嵌套递归。
 */

namespace ch23_5 {

  namespace case1 {
  
    template<typename T, typename U>
    struct Doublify {
    };
    template<int N>
    struct Trouble {
      using LongType = Doublify<typename Trouble<N-1>::LongType, typename Trouble<N-1>::LongType>;
    };
    
    template<>
    struct Trouble<0> {
      using LongType = double;
    };
    
  
    void test() {
      
      Trouble<10>::LongType ouch;
      
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
main_ch23_5()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch23_5::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch23_5::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch23_5::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch23_5::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch23_5::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}