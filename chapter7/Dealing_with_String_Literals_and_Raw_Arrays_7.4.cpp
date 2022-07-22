//
// Created by yangfeng on 2022/7/22.
//
#include "../headers.h"

/*
7.4 处理字符串常量和裸数组
    到目前为止，我们看到了将字符串常量和裸数组用作模板参数时的不同效果：
         按值传递时参数类型会 decay，参数类型会退化成指向其元素类型的指针。
         按引用传递是参数类型不会 decay，参数类型是指向数组的引用。
        
    两种情况各有其优缺点。
        将数组退化成指针，就不能区分它是指向对象的指针还是一个被传递进来的数组。
        另一方面，如果传递进来的是字符串常量，那么类型不退化的话就会带来问题，因为不同长度的字符串的类型是不同的。
        
    比如：
        template<typename T>
        void foo (T const& arg1, T const& arg2)
        {
        }
        foo("hi", "guy"); //ERROR
        
    这里 foo(“hi”, “guy”)不能通过编译，因为”hi”的类型是 char const [3]，
    而”guy”的类型是 charconst [4]，但是函数模板要求两个参数的类型必须相同。
    这种 code 只有在两个字符串常量的长度相同时才能通过编译。
    因此，强烈建议在测试代码中使用长度不同的字符串。
    
    如果将 foo()声明成按值传递的，这种调用可能可以正常运行：
        template<typename T>
        void foo (T arg1, T arg2)
        {
        }
        foo("hi", "guy"); //compiles, but …
        
    但是这样并不能解决所有的问题。反而可能会更糟，编译期间的问题可能会变为运行期间的问题。
    
    考虑如下代码，它用==运算符比较两个传进来的参数：
        template<typename T>
        void foo (T arg1, T arg2)
        {
            if (arg1 == arg2) { //OOPS: compares addresses of passed arrays
            }
        }
        foo("hi", "guy"); //compiles, but …
        
    如上，此时很容易就能知道需要将被传递进来的的字符指针理解成字符串。
    但是情况并不总是这么简单，因为模板还要处理类型可能已经退化过了的字符串常量参数（
    比如它们可能来自另一个按值传递的函数，或者对象是通过 auto 声明的）。
    
    然而，退化在很多情况下是有帮助的，尤其是在需要验证两个对象（
        两个对象都是参数，或者一个对象是参数，并用它给另一个赋值）是否有相同的类型或者可以转换成相同的类型的时候。
        
    这种情况的一个典型应用就是用于完美转发（perfect forwarding）。
    但是使用完美转发需要将参数声明为转发引用。
    这时候就需要使用类型萃取 std::decay<>()显式的退化参数类型。
    可以参考 7.6 节 std::make_pair()这个例子。
    
    注意，有些类型萃取本身可能就会对类型进行隐式退化，比如用来返回两个参数的公共类型的 std::common_type<>。
*/

using namespace std;

namespace ch7_4 {
    namespace case1 {
    
    }
    
    /*
关于字符串常量和裸数组的特殊实现
    有时候可能必须要对数组参数和指针参数做不同的实现。
    此时当然不能退化数组的类型。
    
    为了区分这两种情况，必须要检测到被传递进来的参数是不是数组。
    
    通常有两种方法：
     可以将模板定义成只能接受数组作为参数：
        template<typename T, std::size_t L1, std::size_t L2>
        void foo(T (&arg1)[L1], T (&arg2)[L2])
        {
            T* pa = arg1; // decay arg1
            T* pb = arg2; // decay arg2
            if (compareArrays(pa, L1, pb, L2)) {
            }
        }
        
    参数 arg1 和 arg2 必须是元素类型相同、长度可以不同的两个数组。
    但是为了支持多种不同类型的裸数组，可能需要更多实现方式（参见 5.4 节）。
    
     可以使用类型萃取来检测参数是不是一个数组：
        template<typename T, typename =
        std::enable_if_t<std::is_array_v<T>>>
        void foo (T&& arg1, T&& arg2)
        {
        }
        
    由于这些特殊的处理方式过于复杂，最好还是使用一个不同的函数名来专门处理数组参数。
    或者更近一步，让模板调用者使用 std::vector 或者 std::array 作为参数。
    但是只要字符串还是裸数组，就必须对它们进行单独考虑。
    */
    namespace case2 {
        template <typename T, std::size_t L1, std::size_t L2>
        void func1(T (&arg1)[L1], T(&arg2)[L2]) {
        
        }
        
        template <typename T,
                typename = std::enable_if_t<std::is_array<T>::value>>
        void func2(T&& arg1, T && arg2) {    // 传入右值数组试试
            //T arr;
        
        }
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_dealing_with_string_literals_and_raw_arrays()
{

    return 0;
}
