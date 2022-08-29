//
// Created by yangfeng on 2022/8/25.
//
#include "../headers.h"

using namespace std;

/*
20.7 后记
标记派发（tag dispatch）在 C++中已经存在很久了。它被用于最初版本的 STL 中，而且通常
被和萃取（traits）一起使用。SFINAE 和 std::enable_if 的使用则要晚上很多：本书的第一版
中介绍了 SFINAE 的概念，并展示了其在判断某个成员类型是否存在中的使用。
“enable if”这一技术最早是由 Jaakko J¨arvi, Jeremiah Will-cock, Howard Hinnant, 以及
Andrew Lumsdaine 在[OverloadingProperties]中发布的，在其中他们介绍了 EnableIf 模板，如
何通过 EnableIf（和 DisableIf）实现函数重载，以及如何通过使用 EnableIf 实现类模板的偏
特化。从那时起，EnableIf 以及类似的技术在高端模板库（包含 C++标准库）的实现中就已
五车书馆
266
经变得无处不在了。而且这些技术的流行也促使 C++11 对 SFINAE 进行了扩展（参见第 15.7
节）。Peter Dimov 是第一个注意到在不引入新的模板参数的情况下，函数模板的默认模板
参数（C++11 新特性）可以让 EnableIf 在构造函数模板中的使用变成可能。
Concepts 这一语言特性预期会在 C++17 之后的标准中被引入。它很可能会使一些技术（包含
EnableIf）被废弃掉。与此同时，C++17 的 constexpr if 语句（参见第 8.5 节和第 20.3.3 节）
也正在慢慢渗透进现代模板库中。
 */

namespace ch20_7 {

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
main_ch20_7()
{
  ch20_7::case1::test();

  return 0;
}
