//
// Created by yangfeng on 2022/7/21.
//
#include "../headers.h"

/*
   移动语义（move semantics）是 C++11 引入的一个重要特性。
    在 copy 或者赋值的时候，可以通过它将源对象中的内部资源 move（“steal”）到目标对象，而不是 copy 这些内容。
    当然这样做的前提是源对象不在需要这些内部资源或者状态（因为源对象将会被丢弃）。
    移动语义对模板的设计有重要影响，在泛型代码中也引入了一些特殊的规则来支持移动语义。
 */


namespace ch6_1 {
    
    /*
    假设希望实现的泛型代码可以将被传递参数的基本特性转发出去：
         可变对象被转发之后依然可变。
         Const 对象被转发之后依然是 const 的。
         可移动对象（可以从中窃取资源的对象）被转发之后依然是可移动的。
    不使用模板的话，为达到这一目的就需要对以上三种情况分别编程。
     */
    namespace no_templates {
        class X { };
        void g(X &) { std::cout << "g() for variable\n"; }
        void g(X const &) { std::cout << "g() for constant\n"; }
        void g(X &&) { std::cout << "g() for movable object\n"; }
        
        // let f() forwrad argument val to g()
        void f(X & val) { g(val); }  // val is not-const lvalue => call g(X&)
        void f(X const & val) { g(val); }  // val is const lvalue => call g(X const &)
        void f(X && val) { g(std::move(val)); } // val is not-const lvalue => needs ::move() to call g(X &&)
        
        void test() {
            X v;
            X const cv;
            f(v);
            f(cv);
            f(X());
            f(std::move(v));
        }
    }
    
    /*
        如果试图在泛型代码中统一以上三种情况，会遇到这样一个问题：
            template<typename T>
            void f (T val) {
                g(val);
            }
        这个模板只对前两种情况有效，对第三种用于可移动对象的情况无效。
        基于这一原因，C++11 引入了特殊的规则对参数进行完美转发（perfect forwarding）。
        实现这一目的的惯用方法如下：
            template<typename T>
            void f (T&& val) {
                g(std::forward<T>(val)); // perfect forward val to g()
            }
            
        （按照effective modern c++的说法，当且仅当用来初始化万能引用的形参的实参是“右值”时，
        std::forward<>才将形参转换为右值，否则什么都不做）
        注意 std::move 没有模板参数，并且会无条件地移动其参数；
        而 std::forward<>会跟据被传递参数的具体情况决定是否“转发”其潜在的移动语义。
        
        不要以为模板参数 T 的 T&&和具体类型 X 的 X&&是一样的。
        虽然语法上看上去类似，但是它们适用于不同的规则：
        
         具体类型 X 的 X&&声明了一个右值引用参数。
            只能被绑定到一个可移动对象上（一个prvalue，比如临时对象，
                一个 xvalue，比如通过 std::move()传递的参数，更多细节参见附录 B）。
            它的值总是可变的，而且总是可以被“窃取”。
         模板参数 T 的 T&&声明了一个转发引用（亦称万能引用）。
        可以被绑定到可变、不可变（比如 const）或者可移动对象上。
        在函数内部这个参数也可以是可变、不可变或者指向一个可以被窃取内部数据的值。
        
        注意 T 必须是模板参数的名字。只是依赖于模板参数是不可以的。
        对于模板参数 T，形如typename T::iterator&&的声明只是声明了一个右值引用，不是一个转发引用。
     */
    namespace use_template {
        class X { };
        void g(X &) { std::cout << "g() for variable\n"; }
        void g(X const &) { std::cout << "g() for constant\n"; }
        void g(X &&) { std::cout << "g() for movable object\n"; }
        
        // let f() forwrad argument val to g()
        template <typename T>
        void f(T && val) {
            g(std::forward<T>(val));  // call the right g() for any passwd argument val
        }
        /*
        void f(X & val) { g(val); }  // val is not-const lvalue => call g(X&)
        void f(X const & val) { g(val); }  // val is const lvalue => call g(X const &)
        void f(X && val) { g(std::move(val)); } // val is not-const lvalue => needs ::move() to call g(X &&)
         */
        
        void test() {
            X v;
            X const cv;
            f(v);
            f(cv);
            f(X());
            f(std::move(v));
        }
    }
    
    /* 完美转发同样可以被用于变参模板。 */

    class Tmp {
    public:
    
    };

}

int
//main()
main_perfect_forwarding()
{
    ch6_1::use_template::test();

    return 0;
}
