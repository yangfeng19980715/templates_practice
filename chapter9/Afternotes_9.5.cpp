//
// Created by yangfeng on 2022/7/29.
//
#include "../headers.h"

using namespace std;

/*
后记
    将源代码分成头文件和 CPP 文件是为了遵守唯一定义法则（one-definition rule, ODR）。
     附录A 中对该法则有详实的介绍。
     
    基于 C++编译器实现中的既有惯例，包含模式是一种很务实的解决方案。
    但是在最初的 C++ 实现中情况有所不同：
        模板定义的包含是隐式的，这就给人以源文件和头文件“分离”的错觉（参见第 14 章）。
        
    C++98 通过导出模板（exported templates）支持了模板编译的分离模式（separation model）。
    
    这一分离模式允许被 export 标记的模板声明被声明在头文件里，相应的定义则被实现在 CPP文件里，这一点和常规非 template 代码的情况很相似。
    不同于包含模式，这一模式是一种不基于任何已有实现的理论模式，而且其实现也要远比 C++标准委员会所期待的要复杂。
    直到五年之后这一实现方式才被公布，这期间也没有其它实现方式出现。
    为了保持 C++标准和既有惯例的一致性，C++标准委员会在 C++11 中移除了 export 模式。
    
    对这一方面内容感兴趣的读者可以去读一下背书第一版的 6.3 节和 10.3 节。
 */


namespace ch9_5 {

    class Tmp {
    public:
    
    };

}

int
//main()
main_afternotes()
{

    return 0;
}
