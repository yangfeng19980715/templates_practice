//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

using namespace std;

/*
11.5 推迟计算（Defer Evaluation）
    在实现模板的过程中，有时候需要面对是否需要考虑不完整类型（参见 10.3.1 节）的问题。
    
    考虑如下的类模板：
        template<typename T>
        class Cont {
        private:
            T* elems;
        public:
            …
        };
        
    到目前为止，该 class 可以被用于不完整类型。这很有用，比如可以让其成员指向其自身的类型。
    
        struct Node
        {
            std::string value;
            Cont<Node> next; // only possible if Cont accepts incomplete types
        };
        
    但是，如果使用了某些类型萃取的话，可能就不能将其用于不完整类型了。比如：
        template<typename T>
        class Cont {
        private:
            T* elems;
        public:
            …
            typename
            std::conditional<std::is_move_constructible<T>::value, T&&, T& >::type foo();
        };
        
    这里通过使用 std::conditional（参见 D.5）来决定 foo()的返回类型是 T&&还是 T&。
    决策标准是看模板参数 T 是否支持 move 语义。
    
    问题在于 std::is_move_constructible 要求其参数必须是完整类型（参见 D。3.2 节）。
    使用这种类型的 foo()，struct node 的声明就会报错。
    
    为了解决这一问题，需要使用一个成员模板代替现有 foo()的定义，
    这样就可以将std::is_move_constructible 的计算推迟到 foo()的实例化阶段：
        template<typename T>
        class Cont {
        private:
            T* elems;
        public:
            template<typename D = T>
            typename
            std::conditional<std::is_move_constructible<D>::value, T&&, T&>::type foo();
        };
        
    现在，类型萃取依赖于模板参数 D（默认值是 T），
    并且编译器会一直等到 foo()被以完整类型（比如 Node）为参数调用时，
    才会对类型萃取部分进行计算（此时 Node 是一个完整类型，其只有在定义时才是非完整类型）。
 */

namespace ch11_5 {

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
main_Defer_Evaluations()
{

    return 0;
}