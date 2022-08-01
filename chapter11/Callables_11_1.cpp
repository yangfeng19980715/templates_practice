//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

/*
到目前为止，关于模板的讨论主要是基于直接的任务和应用，集中在某些特性，能力和限制上。
 但是当模板被用于泛型库和框架设计时，其效果更明显，此时必须考虑到一些限制更少的潜在应用。
 虽然本书中几乎所有的内容都可以用于此类设计，接下来还是会重点介绍一些在设计可能会被用于未知类型的便捷组件时应该考虑的问题。

此处并没有穷尽所有的问题，只是总结了目前为止已经介绍的一些特性，
   引入了一些新的特性，同时引用了一些在接下来的章节中才会涉及到的特性。
   希望这能偶促使你继续阅读后面的某些章节。
 */

using namespace std;

/*
一些库包含这样一种接口，客户端代码可以向该类接口传递一个实体，并要求该实体必须被调用。
 
 相关的例子有：
   必须在另一个线程中被执行的操作，
   一个指定该如何处理 hash 值并将其存在 hash 表中的函数（hash 函数），
   一个指定集合中元素排序方式的对象，
   以及一个提供了某些默认参数值的泛型包装器。
   
   标准库也不例外：它定义了很多可以接受可调用对象作为参数的组件。
   
这里会用到一个叫做回调（callback）的名词。
 传统上这一名词被作为函数调用实参使用，我们将保持这一传统。
 比如一个排序函数可能会接受一个回调参数并将其用作排序标准，该回调参数将决定排序顺序。
 
在 C++中，由于一些类型既可以被作为函数调用参数使用，
  也可以按照 f(...)的形式调用，因此可以被用作回调参数：
     函数指针类型
     重载了 operator()的 class 类型（有时被称为仿函数（functors）），这其中包含 lambda 函数
     包含一个可以产生一个函数指针或者函数引用的转换函数的 class 类型这些类型被统称为函数对象类型（function object types），
     其对应的值被称为函数对象（function object）。如果可以接受某种类型的可调用对象的话，泛型代码通常可以从中受益，而模板使其称为可能。
     
 */

namespace ch11_1 {
    
    /*
来看一下标准库中的 for_each()算法是如何实现的（为了避免名字冲突，这里使用“foreach”，为了简单也将不会返回任何值）：
     
        template<typename Iter, typename Callable>
        void foreach (Iter current, Iter end, Callable op)
        {
            while (current != end) {            //as long as not reached the end
                op(*current);                   // call passed operator for current element
                ++current;                      // and move iterator to next element
            }
        }
     
    下面的代码展示了将以上模板用于多种函数对象的情况：
        #include <iostream>
        #include <vector>
        #include "foreach.hpp"
        
        // a function to call:
        void func(int i)
        {
            std::cout << "func() called for: " << i << ’\n’;
        }
        
        // a function object type (for objects that can be used as functions):
        class FuncObj {
        public:
            void operator() (int i) const {                                     //Note: const member function
                std::cout << "FuncObj::op() called for: " << i << ’\n’;
            }
        };
        
        int main()
        {
            std::vector<int> primes = { 2, 3, 5, 7, 11, 13, 17, 19 };
            foreach(primes.begin(), primes.end(),                               // range
                    func);                                                      // function as callable (decays to pointer)
                    
            foreach(primes.begin(), primes.end(),                               // range
                    &func);                                                     // function pointer as callable
                    
            foreach(primes.begin(), primes.end(),                               // range
                    FuncObj());                                                 // function object as callable
                    
            foreach(primes.begin(), primes.end(),                               // range
                    [] (int i) {                                                //lambda as callable
                        std::cout << "lambda called for: " << i << ’\n’;
                    });
        }
        
    详细看一下以上各种情况：
         当把函数名当作函数参数传递时，并不是传递函数本体，而是传递其指针或者引用。
           和数组情况类似（参见 7.4 节），在按值传递时，函数参数退化为指针，
            如果参数类型是模板参数，那么类型会被推断为指向函数的指针。
             和数组一样，按引用传递的函数的类型不会 decay。
             但是函数类型不能真正用 const 限制。
             如果将 foreach()的最后一个参数的类型声明为 Callable const &，const 会被省略。（通常而言，在主流 C++代码中很少会用到函数的引用。）
             
         在第二个调用中，函数指针被显式传递（传递了一个函数名的地址）。
            这和第一中调用方式相同（函数名会隐式的 decay 成指针），但是相对而言会更清楚一些。
            
     如果传递的是仿函数，就是将一个类的对象当作可调用对象进行传递。
        通过一个 class类型进行调用通常等效于调用了它的 operator()。因此下面这样的调用：
        op(*current);
          会被转换成：
        op.operator()(*current);                                               // call operator() with parameter *current for op
        
    注意在定义 operator()的时候最好将其定义成 const 成员函数。
     否则当一些框架或者库不希望该调用会改变被传递对象的状态时，会遇到很不容易 debug 的 error。
     
    对于 class 类型的对象，有可能会被转换为指向 surrogate call function（参见 C.3.5）的指针或者引用。
     此时，下面的调用：
        op(*current);
    会被转换为：
        (op.operator F())(*current);
    其中 F 就是 class 类型的对象可以转换为的，指向函数的指针或者指向函数的引用的类型。
    
     Lambda 表达式会产生仿函数（也称闭包），因此它与仿函数（重载了 operator()的类）的情况没有不同。
       不过 Lambda 引入仿函数的方法更为简便，因此它们从 C++11 开始变得很常见。
       
    有意思的是，以[]开始的 lambdas（没有捕获）会产生一个向函数指针进行转换的运算符。
     但是它从来不会被当作 surrogate call function，因为它的匹配情况总是比常规闭包的 operator()要差。
     */
    
    namespace case1 {
    
    }
    
    /*
处理成员函数以及额外的参数
    在以上例子中漏掉了另一种可以被调用的实体：成员函数。
     这是因为在调用一个非静态成员函数的时候需要像下面这样指出对象：
       object.memfunc(...)或者 ptr->memfunc(...)，
     这和常规情况下的直接调用方式不同：func(...)。
     
    幸运的是，从 C++17 开始，标准库提供了一个工具：std::invoke()，
     它非常方便的统一了上面的成员函数情况和常规函数情况，这样就可以用同一种方式调用所有的可调用对象。
     
    下面代码中 foreach()的实现使用了 std::invoke()：
        #include <utility>
        #include <functional>
        
        template<typename Iter, typename Callable, typename… Args>
        void foreach (Iter current, Iter end, Callable op, Args const&…args)
        {
            while (current != end) {                        //as long as not reached the end of the elements
                std::invoke(op,                             //call passed callable with
                        args…,                              //any additional args
                        *current);                          // and the current element
                    ++current;
            }
        }
        
    这里除了作为参数的可调用对象，foreach()还可以接受任意数量的参数。
     然后 foreach()将参数传递给 std::invoke()。
     
     Std::invoke()会这样处理相关参数：
     如果可调用对象是一个指向成员函数的指针，它会将 args...中的第一个参数当作 this 对象（不是指针）。
        Args...中其余的参数则被当做常规参数传递给可调用对象。
     否则，所有的参数都被直接传递给可调用对象。
       注意这里对于可调用对象和 agrs..都不能使用完美转发（perfect forward）：
       因为第一次调用可能会 steal(偷窃)相关参数的值，导致在随后的调用中出现错误。
       
    现在既可以像之前那样调用 foreach()，也可以向它传递额外的参数，而且可调用对象可以是一个成员函数。
    
    正如下面的代码展现的那样：
        #include <iostream>
        #include <vector>
        #include <string>
        #include "foreachinvoke.hpp"
        
        // a class with a member function that shall be called
        class MyClass {
        public:
            void memfunc(int i) const {
                std::cout << "MyClass::memfunc() called for: " << i << ’\n’;
            }
        };
        
        int main()
        {
            std::vector<int> primes = { 2, 3, 5, 7, 11, 13, 17, 19 };
            
            // pass lambda as callable and an additional argument:
            foreach(primes.begin(), primes.end(),                   //elements for 2nd arg of lambda
                [](std::string const& prefix, int i) {              //lambda to call
                    std::cout << prefix << i << ’\n’;
                },
                "- value:");                                        //1st arg of lambda
            
            // call obj.memfunc() for/with each elements in primes passed as argument
            
            MyClass obj;
            foreach(primes.begin(), primes.end(),                   //elements used as args
                    &MyClass::memfunc,                              //member function to call
                    obj);                                           // object to call memfunc() for
        }
        
    第一次调用 foreach()时，第四个参数被作为 lambda 函数的第一个参数传递给 lambda，
     而vector 中的元素被作为第二个参数传递给 lambda。
     第二次调用中，第三个参数 memfunc()被第四个参数 obj 调用。
     
    关于通过类型萃取判断一个可调用对象是否可以用于 std::invode()的内容，请参见 D.3.1 节。
     */
    namespace case2 {
        template<typename Iter, typename Callable, typename... Args>
        void foreach(Iter current, Iter end, Callable op, Args const &... args) {
            while (current != end) {
                std::invoke(op, args..., *current);
                ++current;
            }
        }
        
        class MyClass {
        public:
            void memfunc(int i) const {
                std::cout << "MyClass::memfunc() called for: " << i << '\n';
            }
        };
    
        void test_func() {
            std::vector<int> primes = {2, 3, 5, 7, 11, 13, 17, 19};
        
            foreach(primes.begin(),
                    primes.end(),
                    [](std::string const &prefix, int i) { std::cout << prefix << i << '\n'; },
                    "- value:"
            );
            
            cout << "***********************************line***********************************" << endl;
        
            MyClass obj;
            foreach(primes.begin(), primes.end(), &MyClass::memfunc, obj);
        }
    
    }
    
    /*
函数调用的包装
    Std::invoke()的一个常规用法是封装一个单独的函数调用（比如：记录相关调用，测量所耗时常，或者准备一些上下文信息（比如为此启动一个线程））。
    
    此时可以通过完美转发可调用对象以及被传递的参数来支持移动语义：
        #include <utility> // for std::invoke()
        #include <functional> // for std::forward()
        
        template<typename Callable, typename… Args>
        decltype(auto) call(Callable&& op, Args&&… args)
        {
            return std::invoke(std::forward<Callable>(op),              //passed callable with
                std::forward<Args>(args)…);                             // any additional args
        }
        
    一个比较有意思的地方是该如何处理被调用函数的返回值，才能将其“完美转发”给调用者。
    
     [[
       auto 作为返回值，会导致类型退化， decltype(auto)避免了这种退化。
       auto(可有cv限定符)一定会推导出返回类型为对象类型，并且应用数组到指针，函数到指针隐式转换。
       auto加上 & 或者 &&（可有cv限定符）一定会推导出返回类型为引用类型。
       decltype(auto)可以推导出对象类型，也可以推导出引用类型。具体取决于decltype应用到return语句中表达式的结果。
     ]]
    为了能够返回引用（比如 std::ostream&）,需要使用 decltype(auto)而不是 auto：
        template<typename Callable, typename… Args>
        decltype(auto) call(Callable&& op, Args&&… args)
     
    decltype(auto)（在 C++14 中引入）是一个占位符类型，它根据相关表达式决定了变量、返回值、或者模板实参的类型。
     详情请参考 15.10.3 节。
     
    如果你想暂时的将 std::invoke()的返回值存储在一个变量中，并在做了某些别的事情后将其
    返回（比如处理该返回值或者记录当前调用的结束），也必须将该临时变量声明为decltype(auto)类型：
        decltype(auto) ret{std::invoke(std::forward<Callable>(op),
        std::forward<Args>(args)…)};
        …
        return ret;
        
    注意这里将 ret 声明为 auto &&是不对的。
     Auto&&作为引用会将变量的生命周期扩展到作用域的末尾（参见 11.3 节），但是不会扩展到超出 return 的地方。
     
    不过即使是使用 decltype(auto)也还是有一个问题：如果可调用对象的返回值是 void，
       那么将 ret 初始化为 decltype(auto)是不可以的，这是因为 void 是不完整类型。
       
    此时有如下选择：
     在当前行前面声明一个对象，并在其析构函数中实现期望的行为。
     比如：
        struct cleanup {
            ~cleanup() {
            … //code to perform on return
            }
        } dummy;
        return std::invoke(std::forward<Callable>(op), std::forward<Args>(args)…);
        
     分别实现 void 和非 void 的情况：
        #include <utility>                          // for std::invoke()
        #include <functional>                       // for std::forward()
        #include <type_traits>                      // for std::is_same<> and invoke_result<>
        
        template<typename Callable, typename… Args>
        decltype(auto) call(Callable&& op, Args&&… args)
        {
            if constexpr(std::is_same_v<std::invoke_result_t<Callable,
            Args…>, void>) {                        // return type is void:
                std::invoke(std::forward<Callable>(op),
                std::forward<Args>(args)…);
                …
                return;
            }
            else
            {
                // return type is not void:
                decltype(auto) ret {
                    std::invoke(std::forward<Callable>(op),
                    std::forward<Args>(args)…)
                    };
                    …
                return ret;
            }
        }
        
    其中：
    if constexpr(std::is_same_v<std::invoke_result_t<Callable, Args…>, void>)
    在编译期间检查使用 Args...的 callable 的返回值是不是 void 类型。
     关于 std::invoke_result<> 的细节请参见 D.3.1 节。
     
    后续的 C++版本可能会免除掉这种对 void 的特殊操作（参见 17.7 节）。
     */
    
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
main_callables_11_1()
{
    ch11_1::case2::test_func();

    return 0;
}
