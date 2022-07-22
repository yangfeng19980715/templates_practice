//
// Created by yangfeng on 2022/7/22.
//
#include "../headers.h"

using namespace std;
/*
    返回值也可以被按引用或者按值返回。
    但是按引用返回可能会带来一些麻烦，因为它所引用的对象不能被很好的控制。
    
    不过在日常编程中，也有一些情况更倾向于按引用返回：
         返回容器或者字符串中的元素（比如通过[]运算符或者 front()方法访问元素）
         允许修改类对象的成员
         为链式调用返回一个对象（比如>>和<<运算符以及赋值运算符）
        另外对成员的只读访问，通常也通过返回 const 引用实现。
        
    但是如果使用不当，以上几种情况就可能导致一些问题。
    比如：
        std::string* s = new std::string("whatever");
        auto& c = (*s)[0];
        delete s;
        std::cout << c;         //run-time ERROR
        
    这里声明了一个指向字符串中元素的引用，但是在使用这个引用的地方，
      对应的字符串却不存在了（成了一个悬空引用），这将导致未定义的行为。
      
    这个例子看上去像是人为制造的（一个有经验的程序员应该可以意识到这个问题），但是情况也不都是这么明显。
    比如：
        auto s = std::make_shared<std::string>("whatever");
        auto& c = (*s)[0];
        s.reset();
        std::cout << c;         //run-time ERROR
        
    因此需要确保函数模板采用按值返回的方式。
    但是正如接下来要讨论的，使用函数模板 T 作为返回类型并不能保证返回值不会是引用，
      因为 T 在某些情况下会被隐式推断为引用类型：
        template<typename T>
        T retR(T&& p)               // p is a forwarding reference
        {
            return T{…};            // OOPS: returns by reference when called for lvalues
        }
        
    即使函数模板被声明为按值传递，也可以显式地将 T 指定为引用类型：
        template<typename T>
        T retV(T p) //Note: T might become a reference
        {
            return T{…}; // OOPS: returns a reference if T is a reference
        }
        int x;
        retV<int&>(x); // retT() instantiated for T as int&
        
    安全起见，有两种选择：
     用类型萃取 std::remove_reference<>（参见 D.4 节）将 T 转为非引用类型：
        template<typename T>
        typename std::remove_reference<T>::type retV(T p)
        {
            return T{…}; // always returns by value
        }
    Std::decay<>（参见 D.4 节）之类的类型萃取可能也会有帮助，因为它们也会隐式的去掉类型的引用。
    
     将返回类型声明为 auto，从而让编译器去推断返回类型，这是因为 auto 也会导致类型退化：
        template<typename T>
        auto retV(T p) // by-value return type deduced by compiler
        {
            return T{…}; // always returns by value
        }
*/

namespace ch7_5 {
    
    namespace case1 {
        void test_func() {
            std::string* s = new std::string("hello");
            auto & c = (*s)[0];
            delete s;
            std::cout << c;  // run-time error
        
        }
        
        void test_func2() {
            auto s = std::make_shared<std::string>("whatever");
            auto &c = (*s)[0];
            s.reset();
            std::cout << c;  // run-time error
        }
        
    }
    
    namespace case2 {
        
        class Test {
        public:
            int num;
        public:
            Test(int _num) : num(_num) { }
            Test() { num = -1; }
        };
        
        template <typename T>
        T retR(T && p) {                // p is a forwarding reference
            return T{ };                 // OOPS: return by reference when called for lvalues
        }
        
        template <typename T>
        typename std::remove_reference<T>::type retV2(T && p) {
            return T{ };
        }
    
        template <typename T>
        auto retV3(T && p) {
            return T{ };
        }
    
    }
    

    class Tmp {
    public:
    
    };

}

int
//main()
main_dealing_with_return_values()
{
    //ch7_5::case1::test_func2();
    ch7_5::case2::Test t1 { 1 };
    auto t4 = ch7_5::case2::retR(ch7_5::case2::Test{});

    return 0;
}