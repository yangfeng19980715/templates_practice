//
// Created by yangfeng on 2022/7/22.
//
#include "../headers.h"

/*

 从一开始，C++就提供了按值传递（call-by-value）和按引用传递（call-by-reference）两种参数传递方式，
   但是具体该怎么选择，有时并不容易确定：
     通常对复杂类型用按引用传递的成本更低，但是也更复杂。
     
     C++11 又引入了移动语义（move semantics），也就是说又多了一种按引用传递的方式：
     
    1. X const &（const 左值引用）
        参数引用了被传递的对象，并且参数不能被更改。
        
    2. X &（非 const 左值引用）
        参数引用了被传递的对象，但是参数可以被更改。
        
    3. X &&（右值引用）
        参数通过移动语义引用了被传递的对象，并且参数值可以被更改或者被“窃取”。
        仅仅对已知的具体类型，决定参数的方式就已经很复杂了。
        在参数类型未知的模板中，就更难选择合适的传递方式了。
        
    不过在 1.6.1 节中，我们曾经建议在函数模板中应该优先使用按值传递，除非遇到以下情况：
         对象不允许被 copy。
         参数被用于返回数据。
         参数以及其所有属性需要被模板转发到别的地方。
         可以获得明显的性能提升。

*/


using namespace std;

namespace ch7_1 {
    
    /*
        当按值传递参数时，原则上所有的参数都会被拷贝。
        因此每一个参数都会是被传递实参的一份拷贝。
        对于 class 的对象，参数会通过 class 的拷贝构造函数来做初始化。

        调用拷贝构造函数的成本可能很高。但是有多种方法可以避免按值传递的高昂成本：
            事实上编译器可以通过移动语义（move semantics）来优化掉对象的拷贝，
            这样即使是对复杂类型的拷贝，其成本也不会很高。
    
     如果定义一个 std::string 对象并将其用于上面的函数模板：
        std::string s = "hi";
        printV(s);
        
        模板参数 T 被实例化为 std::string，实例化后的代码是：
            void printV (std::string arg)
            {
            …
            }
        在传递字符串时，arg 变成 s 的一份拷贝。
        此时这一拷贝是通过 std::string 的拷贝构造函数创建的，这可能会是一个成本很高的操作，
        因为这个拷贝操作会对源对象做一次深拷贝，它需要开辟足够的内存来存储字符串的值。
        但是并不是所有的情况都会调用拷贝构造函数。
        
    考虑如下情况：
     
        std::string returnString();
        std::string s = "hi";
        
        printV(s); //copy constructor
        
        printV(std::string("hi"));      //copying usually optimized away (if not, move constructor)
        
        printV(returnString());         // copying usually optimized away (if not, move constructor)
        
        printV(std::move(s));           // move constructor
        
        在第一次调用中，被传递的参数是左值（lvalue），因此拷贝构造函数会被调用。
        
        但是在第二和第三次调用中，被传递的参数是纯右值（prvalue，pure right value，临时对象或者某个
            函数的返回值，参见附录 B），此时编译器会优化参数传递，使得拷贝构造函数不会被调用。
            
        从 C++17 开始，C++标准要求这一优化方案必须被实现。
        在 C++17 之前，如果编译器没有优化掉这一类拷贝，它至少应该先尝试使用移动语义，这通常也会使拷贝成本变得比较低廉。
        
        在最后一次调用中，被传递参数是 xvalue（一个使用了 std::move()的已经存在的非 const 对象），
            这会通过告知编译器我们不在需要 s 的值来强制调用移动构造函数（move constructor）。
        
        综上所述，在调用 printV()（参数是按值传递的）的时候，
            只有在被传递的参数是 lvalue（对象在函数调用之前创建，并且通常在之后还会被用到，而且没有对其使用 std::move()）时，
            调用成本才会比较高。
            不幸的是，这唯一的情况也是最常见的情况，因为我们几乎总是先创建一个对象，然后在将其传递给其它函数。
    */
    namespace case1 {
        class Test {
        public:
            int num;
        public:
            explicit Test(int _num = 0) : num(_num) { cout << "ctor" << endl; }
            Test(Test const & t) : num(t.num) { cout << "copy ctor" << endl;}
            Test(Test && t) noexcept : num(t.num) { cout << "move ctor" << endl; }
            
        };
        
        template <typename T>
        void printV(T arg) {  }
        
        Test getTestObj() { return Test{ 1 }; }
        
        void test_func() {
            
            // Test t2 { 2 };
            // printV(t2);              // copy ctor
            // printV(Test{ 3 });       // no copy
            // printV(getTestObj());    // no copy
            // printV(std::move(t2));
            
            
        }
        
        
        
    }
    
    /*
    
 按值传递会导致类型退化（decay）
    关于按值传递，还有一个必须被讲到的特性：
     当按值传递参数时，参数类型会退化（decay）。
     也就是说，裸数组会退化成指针，
         const 和 volatile 等限制符会被删除（就像用一个值去初始化一个用 auto 声明的对象那样):
     
    */
    
    namespace case2 {
        
        /*
        当传递字符串常量“hi”的时候，其类型 char const[3]退化成 char const *，
         这也就是模板参数 T 被推断出来的类型。
         此时模板会被实例化成：
            void printV (char const* arg)
            {
            }
        */
        template <typename T>
        void printV(T arg) {
            printf("%d\n", sizeof(arg));
        }
        
        void test_func() {
            char const arr[3] = "hi";
            printV(arr);
        }
        
    }
    

    class Tmp {
    public:
    
    };

}

int
//main()
main_passing_by_value()
{
    ch7_1::case1::test_func();

    return 0;
}
