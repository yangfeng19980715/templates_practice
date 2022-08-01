//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

/*
作为模板参数的引用
    虽然不是很常见，但是模板参数的类型依然可以是引用类型。
    
    比如：
        #include <iostream>
        template<typename T>
        
        void tmplParamIsReference(T) {
            std::cout << "T is reference: " << std::is_reference_v<T> << ’\n’;
        }
        
        int main()
        {
            std::cout << std::boolalpha;
            int i;
            int& r = i;
            tmplParamIsReference(i);                // false
            tmplParamIsReference(r);                // false
            tmplParamIsReference<int&>(i);          // true
            tmplParamIsReference<int&>(r);          // true
        }
        
    即使传递给 tmplParamIsReference()的参数是一个引用变量，
    T 依然会被推断为被引用的类型（因为对于引用变量 v，表达式 v 的类型是被引用的类型，表达式（expression）的类型永远
       不可能是引用类型）。
       
    不过我们可以显示指定 T 的类型化为引用类型：
        tmplParamIsReference<int&>(r);
        tmplParamIsReference<int&>(i);
        
    这样做可以从根本上改变模板的行为，不过由于这并不是模板最初设计的目的，
      这样做可能会触发错误或者不可预知的行为。
      
    考虑如下例子：
        template<typename T, T Z = T{}>
        class RefMem {
        private:
            T zero;
        public:
            RefMem() : zero{Z} {  }
        };
        
        int null = 0;
        
        int main()
        {
            RefMem<int> rm1, rm2;
            rm1 = rm2;                              // OK
            RefMem<int&> rm3;                       // ERROR: invalid default value for N
            RefMem<int&, 0> rm4;                    // ERROR: invalid default value for N extern int null;
            RefMem<int&,null> rm5, rm6;
            rm5 = rm6;                              // ERROR: operator= is deleted due to reference member
        }
        
    此处模板的模板参数为 T，其非类型模板参数 z 被进行了零初始化。
    用 int 实例化该模板会获得预期的行为。
    
    但是如果尝试用引用对其进行实例化的话，情况就有点复杂了：
     非模板参数的默认初始化不在可行。
     不再能够直接用 0 来初始化非参数模板参数。
     最让人意外的是，赋值运算符也不再可用，因为对于具有非 static 引用成员的类，其默赋值运算符会被删除掉。
    
    而且将引用类型用于非类型模板参数同样会变的复杂和危险。
    
    考虑如下例子：
        #include <vector>
        #include <iostream>
        
        template<typename T, int& SZ>               // Note: size is reference
        class Arr {
        private:
            std::vector<T> elems;
        public:
            Arr() : elems(SZ) {  }                  //use current SZ as initial vector size
        
            void print() const {
                for (int i=0; i<SZ; ++i) {          //loop over SZ elements
                    std::cout << elems[i] << ’ ’;
                }
            }
        };
        
        int size = 10;
        
        int main()
        {
            Arr<int&,size> y;               // compile-time ERROR deep in the code of class std::vector<>
            Arr<int,size> x;                // initializes internal vector with 10 elements
            x.print();                      // OK
            size += 100;                    // OOPS: modifies SZ in Arr<>
            x.print();                      // run-time ERROR: invalid memory access: loops over 120 elements
        }
        
    其中尝试将 Arr 的元素实例化为引用类型会导致 std::vector<>中很深层次的错误，
    因为其元素类型不能被实例化为引用类型：
        Arr<int&,size> y;                   // compile-time ERROR deep in the code of class
        std::vector<>
        
    正如 9.4 节介绍的那样，这一类错误通常又臭又长，
    编译器会报出整个模板实例化过程中所有的错误：
        从模板一开始实例化的地方，一直到模板定义中真正触发错误的地方。
        
    可能更糟糕的是将引用用于 size 这一类参数导致的运行时错误：
      可能在容器不知情的情况下，自身的 size 却发生了变化（比如 size 值变得无效）。
      
    如下这样使用 size 的操作（比如print）就很可能会导致未定义的行为（导致程序崩溃甚至更糟糕）：
        int size = 10;
        …
        Arr<int,size> x;            // initializes internal vector with 10 elements
        size += 100;                // OOPS: modifies SZ in Arr<>
        x.print();                  // run-time ERROR: invalid memory access: loops over 120 elements
        
    注意这里并不能通过将 SZ 声明为 int const &来修正这一错误，因为 size 本身依然是可变的。
    看上去这一类问题根本就不会发生。
    但是在更复杂的情况下，确实会遇到此类问题。
    
    比如在C++17 中，非类型模板参数可以通过推断得到：
        template<typename T, decltype(auto) SZ>
        class Arr;
        
    使用 decltype(auto)很容易得到引用类型，因此在这一类上下文中应该尽量避免使用 auto。
    
    详情请参见 15.10.3 节。
    基于这一原因，C++标准库在某些情况下制定了很特殊的规则和限制。
    比如：
    
     在模板参数被用引用类型实例化的情况下，为了依然能够正常使用赋值运算符，
    std::pair<>和 std::tuple<>都没有使用默认的赋值运算符，而是做了单独的定义。
    
    比如：
        namespace std {
            template<typename T1, typename T2>
            struct pair {
                T1 first;
                T2 second;
                …
                // default copy/move constructors are OK even with references:
                pair(pair const&) = default;
                pair(pair&&) = default;
                …
                // but assignment operator have to be defined to be available with references:
                pair& operator=(pair const& p);
                pair& operator=(pair&& p) noexcept(…);
                …
            };
        }
        
     由于这些副作用可能导致的复杂性，在 C++17 中用引用类型实例化标准库模板 std::optional<>和 std::variant<>的过程看上去有些古怪。
        为了禁止用引用类型进行实例化，一个简单的 static_assert 就够了：
        template<typename T>
        class optional
        {
            static_assert(!std::is_reference<T>::value, "Invalid
                                instantiation of optional<T> for references");
        };
    通常引用类型和其他类型有很大不同，并且受一些语言规则的限制。这会影响对调用参数的
    声明（参见第 7 章）以及对类型萃取的定义（参见 19.6.1 节）。
 */

using namespace std;

namespace ch11_4 {

    namespace case1 {
    }
    
    namespace case2 {
    }
    
    namespace case3 {
    }
    
    namespace case4 {
    }
    
    namespace case5 {
    
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_References_as_Template_Parameters_11_4()
{

    return 0;
}