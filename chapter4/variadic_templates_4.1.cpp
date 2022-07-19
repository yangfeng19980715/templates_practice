//
// Created by yangfeng on 2022/7/18.
//
#include "../headers.h"

using namespace std;

/*
 * 可以将模板参数定义成能够接受任意多个模板参数的情况。
 *   这一类模板被称为变参模板（variadic template）。
 */

namespace ch4_1 {
    
    class Print {
    public:
        
        /*
         * 打印数量和类型都不确定的参数
         */
        void print() { }
        
        // 这里使用了通过模板参数包（template parameter pack）定义的类型“Types”
        template <typename T, typename ... Types>
        void print(T first, Types ... args) {  // args是一个函数参数包(function parameter pack)
            std::cout << first << '\n';  // 打印第一个参数
            print(args...);  // 打印剩下的参数
        }
        
        void test_print() {
            print(1, 2, 3.4, 'h', "hello, world");
        }
    };
    
    class Print2 {
    public:
        /*
         *
         * 变参和非变参模板的重载
         *
         * 当两个函数模板的区别只在于尾部的参数包的时候，
         *   会优先选择没有尾部参数包的那一个函数模板。
         */
        
        template <typename T>
        void print(T arg) {
            cout << arg << '\n';
        }
        
        template <typename T, typename... Types>
        void print(T firstArg, Types... args) {
            print(firstArg);
            print(args...);
        }
    };
    
    
    /*
     * sizeof... 运算符
     *    C++11 为变参模板引入了一种新的 sizeof 运算符：sizeof...。
     *    它会被扩展成参数包中所包含的参数数目。
     *    运算符 sizeof... 既可以用于模板参数包，也可以用于函数参数包。
     */
    class Print3 {
    public:
        template <typename T, typename... Types>
        void print(T firstArg, Types... args) {
            cout << firstArg << '\n';
            cout << sizeof...(Types) << endl;  // print number of remaining types
            cout << sizeof...(args) << endl;   // print number of remaining args
        
        }
        
        void test_print() {
            print(1, string{"hello"}, 3.4f, "world");
        }
    };
    
    /*
     * 这样可能会让你觉得，可以不使用为了结束递归而重载的不接受参数的非模板函数 print()，
     *   只要在没有参数的时候不去调用任何函数就可以了：
        template<typename T, typename… Types>
        void print (T firstArg, Types… args)
        {
            std::cout << firstArg << ’\n’;
            if (sizeof…(args) > 0) { //error if sizeof…(args)==0
                print(args…); // and no print() for no arguments declared
            }
        }
      但是这一方式是错误的，因为通常函数模板中 if 语句的两个分支都会被实例化。
       是否使用被实例化出来的代码是在运行期间（run-time）决定的，而是否实例化代码是在编译期间（compile-time）决定的。
       因此如果在只有一个参数的时候调用 print()函数模板，虽然 args...为空，
       if 语句中的 print(args...)也依然会被实例化，但此时没有定义不接受参数的 print()函数，因此会报错。
       
       不过从 C++17 开始，可以使用编译阶段的 if 语句，
         这样通过一些稍微不同的语法，就可以实现前面想要的功能。
     */
    

    class Tmp {
    public:
    
    };

}

int
//main()
main_variadic_templates()
{
    auto pt = ch4_1::Print3{};
    pt.test_print();
    
    cout << "hello, variadic_templates" << endl;

    return 0;
}

