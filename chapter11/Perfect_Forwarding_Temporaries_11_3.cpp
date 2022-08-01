//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

using namespace std;

/*
11.3 完美转发临时变量
    正如 6.1 节介绍的那样，我们可以使用转发引用（forwarding reference）以及 std::forward<>来完美转发泛型参数：
    
        template<typename T>
        void f (T&& t) // t is forwarding reference
        {
            g(std::forward<T>(t)); // perfectly forward passed argument t to g()
        }
        
    但是某些情况下，在泛型代码中我们需要转发一些不是通过参数传递进来的数据。
    此时我们可以使用 auto &&创建一个可以被转发的变量。
    
    比如，假设我们需要相继的调用 get()和 set() 两个函数，并且需要将 get()的返回值完美的转发给 set():
        template<typename T>void foo(T x)
        {
            set(get(x));
        }
        
    假设以后我们需要更新代码对 get()的返回值进行某些操作，
      可以通过将 get()的返回值存储在一个被声明为 auto &&的变量中实现：
        template<typename T>
        void foo(T x)
        {
            auto&& val = get(x);
            
            // perfectly forward the return value of get() to set():
            set(std::forward<decltype(val)>(val));
        }
        
    这样可以避免对中间变量的多余拷贝。
 */

namespace ch11_3 {

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
main_Perfect_Forwarding_Temporaries_11_3()
{

    return 0;
}