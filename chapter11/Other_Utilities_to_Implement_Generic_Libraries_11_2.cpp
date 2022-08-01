//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

/*
std::invoke()只是 C++标准库提供的诸多有用工具中的一个。在接下来的内容中，我们会介绍其他一些重要的工具。
 */

using namespace std;

namespace ch11_2 {
    /*
类型萃取
    标准库提供了各种各样的被称为类型萃取（type traits）的工具，它们可以被用来计算以及修改类型。
    这样就可以在实例化的时候让泛型代码适应各种类型或者对不同的类型做出不同的响应。
     比如：
     
        #include <type_traits>
        
        template<typename T>
        class C
        {
            // ensure that T is not void (ignoring const or volatile):
            static_assert(!std::is_same_v<std::remove_cv_t<T>,void>,"invalid instantiation of class C for void type");
        
        public:
            template<typename V>
            void f(V&& v) {
                if constexpr(std::is_reference_v<T>) {
                    …               // special code if T is a reference type
                }
                if constexpr(std::is_convertible_v<std::decay_t<V>,T>) {
                    …               // special code if V is convertible to T
                }
                if constexpr(std::has_virtual_destructor_v<V>) {
                    …               // special code if V has virtual destructor
                }
            }
        };
        
    如上所示，通过检查某些条件,可以在模板的不同实现之间做选择。
     在这里用到了编译期的 if 特性，该特性从 C++17 开始可用，作为替代选项，
     这里也可以使用 std::enable_if、部分特例化或者 SFINAE（参见第 8 章）。
     
    但是使用类型萃取的时候需要额外小心：
     其行为可能和程序员的预期不同。
     
    比如：
    std::remove_const_t<int const&>                 // yields int const&
    
    这里由于引用不是 const 类型的（虽然你不可以改变它），这个操作不会有任何效果。
    
    这样，删除引用和删除 const 的顺序就很重要了：
        std::remove_const_t<std::remove_reference_t<int const&>> // int
        std::remove_reference_t<std::remove_const_t<int const&>> // int const
        
    另一种方法是，直接调用：
        std::decay_t<int const&>                                // yields int
        
    但是这同样会让裸数组和函数类型退化为相应的指针类型。
    
    当然还有一些类型萃取的使用是有要求的。
    这些要求不被满足的话，其行为将是未定义的。
    
    比如：
        make_unsigned_t<int>                                    // unsigned int
        make_unsigned_t<int const&>                             // undefined behavior (hopefully error)
        
    某些情况下，结果可能会让你很意外。比如：
        add_rvalue_reference_t<int const>                           // int const&&
        add_rvalue_reference_t<int const&>                          // int const& (lvalueref remains lvalue-ref)
        
    这里我们期望 add_rvalue_reference 总是能够返回一个右值引用，
      但是 C++中的引用塌缩（reference-collapsing rules，参见 15.6.1 节）会令左值引用和右值引用的组合返回一个左值引用。
      
    另一个例子是：
        is_copy_assignable_v<int>                                   // yields true (generally, you can assign an int to an int)
        is_assignable_v<int,int>                                    // yields false (can’t call 42 = 42)
        
    其中 is_copy_assignable 通常只会检查是否能够将一个 int 赋值给另外一个（检查左值的相关操作），
     而 is_assignable 则会考虑值的种类（value category，会检查是否能将一个右值赋值给另外一个）。
     
    也就是说第一个语句等效于：
        is_assignable_v<int&,int&>                                      // yields true
    对下面的例子也是这样：
        is_swappable_v<int>                                             // yields true (assuming lvalues)
        is_swappable_v<int&,int&>                                       // yields true (equivalent to the previous check)
        is_swappable_with_v<int,int>                                    // yields false (taking value category into account)
    综上，在使用时需要额外注意类型萃取的精确定义。相关规则定义在附录 D 中。
    
     */
    
    namespace case1 {
    
    }
    
    /*
    11.2.2 std::addressoff()
    
    函数模板 std::addressof<>()会返回一个对象或者函数的准确地址。
     即使一个对象重载了运算符&也是这样。
     虽然后者中的情况很少遇到，但是也会发生（比如在智能指针中）。
     
    因此，如果需要获得任意类型的对象的地址，那么推荐使用 addressof()：
        template<typename T>
        void f (T&& x)
        {
            auto p = &x; // might fail with overloaded operator &
            auto q = std::addressof(x); // works even with overloaded operator &
        }
     */
    
    namespace case2 {
    
    }
    
    /*
11.2.3 std::declval()
    函数模板 std::declval()可以被用作某一类型的对象的引用的占位符。
     该函数模板没有定义，因此不能被调用（也不会创建对象）。
     
     因此它只能被用作不会被计算的操作数（比如 decltype 和 sizeof）。
     也因此，在不创建对象的情况下，依然可以假设有相应类型的可用对象。
     
    比如在如下例子中，会基于模板参数 T1 和 T2 推断出返回类型 RT：
        #include <utility>
        template<typename T1, typename T2,
        typename RT = std::decay_t<decltype(true ? std::declval<T1>() :
        
        std::declval<T2>())>>
        RT max (T1 a, T2 b)
        {
            return b < a ? a : b;
        }
        
    为了避免在调用运算符?:的时候不得不去调用 T1 和 T2 的（默认）构造函数，
     这里使用了 std::declval，这样可以在不创建对象的情况下“使用”它们。
     
     不过该方式只能在不会做真正的计算时（比如 decltype）使用。
    不要忘了使用 std::decay<>来确保返回类型不会是一个引用，因为 std::declval<>本身返回的是右值引用。
     
     否则，类似 max(1,2)这样的调用将会返回一个 int&&类型。
     
     相关细节请参见 19.3.4节。
     */

    class Tmp {
    public:
    
    };

}

int
//main()
main_Other_Utilities_to_Implement_Generic_Libraries_11_2()
{

    return 0;
}