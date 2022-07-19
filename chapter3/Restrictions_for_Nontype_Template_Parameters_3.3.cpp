//
// Created by yangfeng on 2022/7/18.
//
#include "../headers.h"

using namespace std;

extern char const s03[] = "hello, world";
char const s11[] = "s11";

/* 使用非类型模板参数是有限制的。 (c++20之前)
 *   通常它们只能是整形常量（包含枚举），
 *   指向objects/functions/members 的指针，
 *   objects 或者 functions 的左值引用，
 *   或者是 std::nullptr_t（类型是 nullptr）。
 *
 *  浮点型数值或者 class 类型的对象都不能作为非类型模板参数使用
 */

namespace ch3_3 {
    /*
        template<double VAT>            // ERROR: floating-point values are not
        double process (double v)       // allowed as template parameters
        {
            return v * VAT;
        }
        
        template<std::string name>      // ERROR: class-type objects are not
        class MyClass {                 // allowed as template parameters
            …
        };
    
    */
    
    template <double VAT>
    double process(double v) {
        return v * VAT;
    }
    
    
    /* 当传递对象的指针或者引用作为模板参数时，
     *  对象不能是字符串常量，
     *  临时变量或者数据成员以及其它子对象。
     *
     *  由于在 C++17 之前，C++版本的每次更新都会放宽以上限制，
     *    因此还有一些针对不同版本的限制：
           在 C++11 中，对象必须要有外部链接。
           在 C++14 中，对象必须是外部链接或者内部链接。
    */
    
    template <char const * name>
    class MyClass {
    public:
        void func() {
            cout << name << endl;
        }
        
    };
    
    void func() {
        cout << process<1.1>(2.2) << endl;
        static char const s17[] = "s17";
        MyClass<s03> mc03;   // ok, all version
        MyClass<s11> mc11;
        MyClass<s17> mc17;
        mc17.func();
    }
    
    /*
     * 非类型模板参数可以是任何编译器表达式，比如：
     *   template< int I, bool B>
     *   class C;
     *
     *   C< sizeof(int) + 4, sizeof(int) == 4> c;
     *
     *   不过如果在表达式中使用了 operator > ，就必须将相应表达式放在括号里面，
     *      否则 > 会被作为模板参数列表末尾的 > ，从而截断了参数列表。
     */
    
    template <int I, bool B>
    class C {
    public:
        void func() {
            cout << "hello, class C" << endl;
        }
    };
    
    void func_C() {
        C<42, (sizeof(int) > 4) > c;
        c.func();
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_restrictions_for_nontype_template_parameters()
{
    ch3_3::func_C();

    return 0;
}
