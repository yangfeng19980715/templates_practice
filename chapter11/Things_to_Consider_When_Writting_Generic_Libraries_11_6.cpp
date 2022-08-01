//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

/*
在写泛型库时需要考虑的事情
    下面让我们列出在实现泛型库的过程中需要记住的一些事情：
         在模板中使用转发引用来转发数值（参见 6.1 节）。如果数值不依赖于模板参数，就使用 auto &&（参见 11.3）。
        
         如果一个参数被声明为转发引用，并且传递给它一个左值的话，那么模板参数会被推断为引用类型（参见 15.6.2 节或者《Effective Morden C++》）。
        
         在需要一个依赖于模板参数的对象的地址的时候，最好使用 std::addressof()来获取地址，
            这样能避免因为对象被绑定到一个重载了 operator &的类型而导致的意外情况（参见11.2.2）。
            
         对于成员函数，需要确保它们不会比预定义的 copy/move 构造函数或者赋值运算符更能匹配某个调用（参见 6.4 节）。
        
         如果模板参数可能是字符串常量，并且不是被按值传递的，那么请考虑使用 std::decay（参见 7.4 节以及附录 D.4）
        
         如果你有被用于输出或者即用于输入也用于输出的、依赖于模板参数的调用参数，
            请为可能的、const 类型的模板参数做好准备（参见 7.2.2 节）。
        
         请为将引用用于模板参数的副作用做好准备（参见 11.4 节）。尤其是在你需要确保返回类型不会是引用的时候（参见 7.5 节）。
        
         请为将不完整类型用于嵌套式数据结构这一类情况做好准备（参见 11.5 节）。
        
         为所有数组类型进行重载，而不仅仅是 T[SZ]（参见 5.4 节）。
 */


using namespace std;

namespace ch11_6 {

    namespace case1 {
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
main_Things_to_Consider_When_Writting_Generic_Libraries_11_6()
{

    return 0;
}