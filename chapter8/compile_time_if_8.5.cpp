//
// Created by yangfeng on 2022/7/27.
//

#include "../headers.h"

using namespace std;

/*
8.5 编译期 if
    部分特例化，SFINAE 以及 std::enable_if 可以一起被用来禁用或者启用某个模板。
      而 C++17又在此基础上引入了同样可以在编译期基于某些条件禁用或者启用相应模板的编译期 if 语句。
       通过使用 if constexpr(...)语法，
        编译器会使用编译期表达式来决定是使用 if 语句的 then 对应的部分还是 else 对应的部分。
        
    作为第一个例子，考虑 4.1.1 节介绍的变参函数模板 print()。它用递归的方法打印其参数（可能是任意类型）。
     如果使用 constexp if，就可以在函数内部决定是否要继续递归下去，而不用再单独定义一个函数来终结递归：
        template<typename T, typename… Types>
        void print (T const& firstArg, Types const&… args)
        {
            std::cout << firstArg << ’\n’;
            if constexpr(sizeof…(args) > 0) {
                print(args…);               //code only available if sizeof…(args)>0 (since C++17)
            }
        }
        
    这里如果只给 print()传递一个参数，那么 args...就是一个空的参数包，
      此时 sizeof...(args)等于0。
       这样 if 语句里面的语句就会被丢弃掉，也就是说这部分代码不会被实例化。
       因此也就不再需要一个单独的函数来终结递归。
       
    事实上上面所说的不会被实例化，意思是对这部分代码只会进行第一阶段编译，
      此时只会做语法检查以及和模板参数无关的名称检查（参见 1.1.3 节）。
      
      比如：
        template<typename T>
        void foo(T t)
        {
            if constexpr(std::is_integral_v<T>) {
                if (t > 0) {
                    foo(t-1); // OK
                }
            }
            else {
                undeclared(t); // error if not declared and not discarded (i.e. T is not integral)
                undeclared(); // error if not declared (even if discarded)
                static_assert(false, "no integral"); // always asserts (even if discarded)
                static_assert(!std::is_integral_v<T>, "no integral"); //OK
            }
        }
        
    此处 if constexpr 的使用并不仅限于模板函数，而是可以用于任意类型的函数。
    它所需要的只是一个可以返回布尔值的编译期表达式。
    
    比如：
        int main()
        {
            if constexpr(std::numeric_limits<char>::is_signed {
                foo(42); // OK
            }else {
                undeclared(42); // error if undeclared() not declared
                static_assert(false, "unsigned"); // always asserts (even if discarded)
                static_assert(!std::numeric_limits<char>::is_signed, "char is unsigned"); //OK
            }
        }
    
    利用这一特性，也可以让 8.2 节介绍的编译期函数 isPrime()在非类型参数不是质数的时候执行一些额外的代码：
        template<typename T, std::size_t SZ>
        void foo (std::array<T,SZ> const& coll)
        {
            if constexpr(!isPrime(SZ)) {
                //special additional handling if the passed array has no prime number as size
            }
        }
        
    更多细节请参见 14.6 节。
*/

namespace ch8_5 {
    
    namespace case1 {
        
        template <typename T, typename... Types>
        void print(T const & firstArg, Types const& ... args) {
            std::cout << firstArg << '\n';
            if constexpr (sizeof...(args) > 0) {
                print(args...);
            }
        }
        
    }
    
    class Tmp {
    public:
    
    };
    
}

namespace test_ch8_5 {
    void test_case1() {
        ch8_5::case1::print("hello", 1, 'c', 3.4f);
    }
}

int
//main()
main_compile_time_if()
{
    test_ch8_5::test_case1();
    
    return 0;
}
