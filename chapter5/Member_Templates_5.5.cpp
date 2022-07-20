//
// Created by yangfeng on 2022/7/20.
//

#include "../headers.h"

using namespace std;

/*
  类的成员也可以是模板，对嵌套类和成员函数都是这样。
  这一功能的作用和优点同样可以通过 Stack<>类模板得到展现。
  
  通常只有当两个 stack 类型相同的时候才可以相互赋值（stack的类型相同说明它们的元素类型也相同）。
      即使两个 stack 的元素类型之间可以隐式转换，也不能相互赋值：
        Stack<int> intStack1, intStack2;    // stacks for ints
        Stack<float> floatStack;            // stack for floats
        
        intStack1 = intStack2;              // OK: stacks have same type
        floatStack = intStack1;             // ERROR: stacks have different types
    
    默认的赋值运算符要求等号两边的对象类型必须相同，因此如果两个 stack 之间的元素类型
    不同的话，这一条件将得不到满足。
    
    但是，只要将赋值运算符定义成模板，就可以将两个元素类型可以做转换的 stack 相互赋值。
 */

namespace ch5_5 {

    template <typename T>
    class Stack {
    private:
        std::deque<T> elems;
    public:
        void push(T const &);
        void pop();
        T const & top() const ;
        bool empty() const { return elems.empty(); }
        
        // assign stack of element of type T2
        template <typename T2>
        Stack& operator=(Stack<T2> const &);
        
        /*
            以上代码中有如下两点改动：
            1. 赋值运算符的参数是一个元素类型为 T2 的 stack。
            2. 新的模板使用 std::deque<>作为内部容器。这是为了方便新的赋值运算符的定义。
         */
        
        // 为了访问op2的私有成员，可以将其他所有类型的stack模板的实例都定义成友元
        // 由于模板参数的名字不会被用到，因此可以被省略
        template <typename> friend class Stack;
        
        // 这样就可以将赋值运算符定义成如下形式
    /*
        template<typename T>
        template<typename T2>
        Stack<T>& Stack<T>::operator= (Stack<T2> const& op2)
        {
            elems.clear();                  // remove existing elements
            elems.insert(elems.begin(),     // insert at the beginning
                         op2.elems.begin(), // all elements from op2
                         op2.elems.end());
            return *this;
        }
    */
    
    // 对类模板而言，其成员函数只有在被用到的时候才会被实例化。
        
    };
    
    // 新的赋值运算符定义
    template <typename T>
    template <typename T2>
    Stack<T>& Stack<T>::operator=(const Stack<T2> & op2) {
        Stack<T2> tmp(op2);     // create a copy of the assigned stack
        elems.clear();          // remove existing elements
        while (!tmp.empty()) {  // copy all elements
            elems.push_front(tmp.top());
            tmp.pop();
        }
        return *this;
    }
    
    /*
    成员模板的特例化
        成员函数模板也可以被全部或者部分地特例化。比如对下面这个例子：
            class BoolString {
            private:
                std::string value;
            public:
                BoolString (std::string const& s)
                        : value(s) {}
                template<typename T = std::string>
                T get() const { // get value (converted to T)
                    return value;
                }
            };
            
            可以像下面这样对其成员函数模板 get()进行全特例化：
            // full specialization for BoolString::getValue<>() for bool
            template<>
            inline bool BoolString::get<bool>() const {
                return value == "true" || value == "1" || value == "on";
            }
            
            注意我们不需要也不能够对特例化的版本进行声明；只能定义它们。
            由于这是一个定义于头文件中的全实例化版本，
            如果有多个编译单 include 了这个头文件，
            为避免重复定义的错误，必须将它定义成 inline 的。

     */
    class BoolString {
    private:
        std::string value;
    public:
        BoolString (std::string const& s) : value(s) {}
        template<typename T = std::string>
        T get() const { // get value (converted to T)
            return value;
        }
    };
    
    // 可以像下面这样对其成员函数模板 get()进行全特例化：
    
    // full specialization for BoolString::getValue<>() for bool
    template<>
    inline bool BoolString::get<bool>() const {
        return value == "true" || value == "1" || value == "on";
    }
    
    void test_boolString() {
        std::cout << std::boolalpha;
        BoolString s1{"hello"};
        std::cout << s1.get() << '\n';
        std::cout << s1.get<bool>() << '\n';
    }
    
    /*
     .template 的使用
        某些情况下，在调用成员模板的时候需要显式地指定其模板参数的类型。
        这时候就需要使用关键字 template 来确保符号<会被理解为模板参数列表的开始，
        而不是一个比较运算符。
        考虑下面这个使用了标准库中的 bitset 的例子：
        template<unsigned long N>
        void printBitset (std::bitset<N> const& bs) {
            std::cout << bs.template to_string<char,
                    std::char_traits<char>,
                    std::allocator<char>>();
        }
        
        对于 bitset 类型的 bs，调用了其成员函数模板 to_string()，
        并且指定了 to_string()模板的所有模板参数。
        如果没有.template 的话，编译器会将 to_string()后面的<符号理解成小于运算符，
        而不是模板的参数列表的开始。
        
        这一这种情况只有在点号前面的对象依赖于模板参数的时候才会发生。
        在我们的例子中，bs 依赖于模板参数 N。
        .template 标识符（标识符->template 和::template 也类似）只能被用于模板内部，
        并且它前面的对象应该依赖于模板参数。
    */
    
    class TestBitset {
    public:
        template<unsigned long N>
        void printBitset(std::bitset<N> const & bs) {
            std::cout << bs.template to_string<char,
                    std::char_traits<char>,
                    std::allocator<char>>();
        }
    };
    
    /*
     *   泛型 lambdas 和成员模板
     *
    在 C++14 中引入的泛型 lambdas，是一种成员模板的简化。
     对于一个简单的计算两个任意类型参数之和的 lambda：
        [] (auto x, auto y) {
            return x + y;
        }
        
    编译器会默认为它构造下面这样一个类：
        class SomeCompilerSpecificName {
        public:
            SomeCompilerSpecificName(); // constructor only callable by compiler
            template<typename T1, typename T2>
            auto operator() (T1 x, T2 y) const {
                return x + y;
            }
        };
     */
}

int
//main()
main_member_templates()
{
    ch5_5::TestBitset tbs{};
    bitset<4> bs = 0b0011;
    tbs.printBitset<4>(bs);
    
    return 0;
}