//
// Created by yangfeng on 9/7/2022.
//

#include "../headers.h"

using namespace std;

/*
 23.4 计算完整性
从以上的 Sqrt<>的例子可以看出，一个模板元程序可能会包含以下内容：

状态变量：模板参数

循环结构：通过递归实现

执行路径选择：通过条件表达式或者偏特例化实现

整数运算
如果对递归实例化的数量和使用的状态变量的数量不做限制，那么就可以用之来计算任何可
以计算的事情。尽管这样做可能不是很方便。而且，由于模板实例化需要大量的编译器资源，
大量的递归实例化会很快地降低编译器地编译速度，甚至是耗尽可用地硬件资源。C++标准
建议最少应该支持 1024 层的递归实例化，但是并没有强制如此，但是这应该足够大部分（当
然不是全部）模板元编程任务使用了。
因此在实际中，不应该滥用模板元编程。但是在少数情况下，在实现便捷模板方面它又不可
替代。在一些特殊情况下，它们可能被隐藏在常规模板的深处，以尽可能地提高关键算法地
计算性能。
 */


namespace ch23_4 {

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

/*

int
//main()
main_ch23_4()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch23_4::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch23_4::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch23_4::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch23_4::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch23_4::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}
 */
