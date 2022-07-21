//
// Created by yangfeng on 2022/7/21.
//
#include "../headers.h"

using namespace std;

namespace ch6_4 {

/*
    通过使用 enable_if<>可以解决 6.2 节中关于构造函数模板的问题。
    
    我们要解决的问题是：
        当传递的模板参数的类型不正确的时候（比如不是 std::string 或者可以转换成 std::string 的类型），
        禁用如下构造函数模板：
            template<typename STR>
            Person(STR&& n);
    为了这一目的，需要使用另一个标准库的类型萃取，std::is_convertiable<FROM, TO>。
        在 C++17中，相应的构造函数模板的定义如下：
            template<typename STR, typename =
            std::enable_if_t<std::is_convertible_v<STR, std::string>>>
            Person(STR&& n);
            
    如果 STR 可以转换成 std::string，这个定义会扩展成：
            template<typename STR, typename = void>
            Person(STR&& n);
            
    否则这个函数模板会被忽略。
    这里同样可以使用别名模板给限制条件定义一个别名：
            template<typename T>
            using EnableIfString = std::enable_if_t<std::is_convertible_v<T,
                    std::string>>;
            …
            template<typename STR, typename = EnableIfString<STR>>
            Person(STR&& n);
*/

    namespace case1 {
        template<typename T>
        using EnableIfString =
                std::enable_if_t<std::is_convertible_v<T,std::string>>;
        
        class Person {
        private:
            std::string name;
        public:
            
            // generic constructor for passed initial name:
            template<typename STR, typename = EnableIfString<STR>>
            explicit Person(STR&& n)
                    : name(std::forward<STR>(n)) {
                std::cout << "TMPL-CONSTR for " << name << "\n";
            }
            
            // copy and move constructor:
            Person (Person const& p) : name(p.name) {
                std::cout << "COPY-CONSTR Person " << name << "\n";
            }
            Person (Person&& p) : name(std::move(p.name)) {
                std::cout << "MOVE-CONSTR Person " << name << "\n";
            }
        };
        
        void Person_main() {
            std::string s = "sname";
            Person p1(s);               // init with string object => calls TMPL-CONSTR
            Person p2("tmp");           // init with string literal => calls TMPL-CONSTR
            Person p3(p1);              // OK => calls COPY-CONSTR
            Person p4(std::move(p1));   // OK => calls MOVE-CONST
        }
    }
    
/*
禁用某些成员函数
    注意我们不能通过使用 enable_if<>来禁用 copy/move 构造函数以及赋值构造函数。
    这是因为成员函数模板不会被算作特殊成员函数（依然会生成默认构造函数），
        而且在需要使用copy 构造函数的地方，相应的成员函数模板会被忽略掉。
    因此即使像下面这样定义类模板：
        class C {
        public:
            template<typename T>
            C (T const&) { std::cout << "tmpl copy constructor\n"; }
        };
    在需要 copy 构造函数的地方依然会使用预定义的 copy 构造函数：
        C x;
        C y{x}; // still uses the predefined copy constructor (not the member template)
        
    删掉 copy 构造函数也不行，因为这样在需要 copy 构造函数的地方会报错说该函数被删除了。
    
    但是也有一个办法：
        可以定义一个接受 const volatile 的 copy 构造函数并将其标示为 delete。
        
    这样做就不会再隐式声明一个接受 const 参数的 copy 构造函数。
    在此基础上，可以定义一个构造函数模板，
        对于 nonvolatile 的类型，它会优选被选择（相较于已删除的 copy 构造函数）：
        class C
        {
        public:
        …
            // user-define the predefined copy constructor as deleted
            // (with conversion to volatile to enable better matches)
            C(C const volatile&) = delete;
            
            // implement copy constructor template with better match:
            template<typename T>
            C (T const&) {
            std::cout << "tmpl copy constructor\n";
        }
        …
        };
        
    这样即使对常规 copy，也会调用模板构造函数：
    C x;
    C y{x}; // uses the member template
    
    于是就可以给这个模板构造函数添加 enable_if<>限制。
    比如可以禁止对通过 int 类型参数实例化出来的 C<>模板实例进行 copy：
        template<typename T>
        class C
        {
        public:
        …
            // user-define the predefined copy constructor as deleted
            // (with conversion to volatile to enable better matches)
            C(C const volatile&) = delete;
            
            // if T is no integral type, provide copy constructor template with better match:
            template<typename U,
            typename = std::enable_if_t<!std::is_integral<U>::value>>
            C (C<U> const&) {
                …
            }
        …
        };
    
    
*/
    namespace case2 {
        template <typename T>
        class C {
        public:
            C() { cout << "C()" << endl; }
            
            C(C const volatile &) = delete;
            
            /*
            template <typename T>
            C (T const & _c) { cout << "template<T> C(C const &)" << endl; }
             */
            template <typename U,
                    typename = std::enable_if_t<!std::is_integral<U>::value>>
            C(C<U> const & ) {
                cout << "if U is integral, is not allowed to copy" << endl;
            }
            
            
        };
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_using_enable_if()
{
    //ch6_4::case1::Person_main();
    ch6_4::case2::C<std::string> x;
    ch6_4::case2::C y { x };
    

    return 0;
}
