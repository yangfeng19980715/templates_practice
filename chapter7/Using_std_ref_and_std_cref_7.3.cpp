//
// Created by yangfeng on 2022/7/22.
//
#include "../headers.h"

using namespace std;

/*
从 C++11 开始，可以让调用者自行决定向函数模板传递参数的方式。
    如果模板参数被声明成按值传递的，
    调用者可以使用定义在头文件<functional>中的 std::ref()和 std::cref()将参数按引用传递给函数模板。
    
    比如：
        template<typename T>
        void printT (T arg) {
        }
        
        std::string s = "hello";
        printT(s);                      //pass s By value
        printT(std::cref(s));           // pass s “as if by reference”
        
    但是请注意，std::cref()并没有改变函数模板内部处理参数的方式。
    
    相反，在这里它使用了一个技巧：它用一个行为和引用类似的对象对参数进行了封装。
    
    事实上，它创建了一个std::reference_wrapper<>的对象，该对象引用了原始参数，并被按值传递给了函数模板。
    
    Std::reference_wrapper<>可能只支持一个操作：向原始类型的隐式类型转换，该转换返回原始参数对象。
    因此当需要操作被传递对象时，都可以直接使用这个 std::reference_wrapper<>对象。
*/

namespace ch7_3 {
    
    /*
  最后一个调用将一个 std::reference_wrapper<string const>对象按值传递给参数 arg，
      这样 std::reference_wrapper<string const> 对象被传进函数模板并被转换为原始参数类型 std::string。
     
    注意，编译器必须知道需要将 std::reference_wrapper<string const>对象转换为原始参数类型，才会进行隐式转换。
     因此 std::ref()和 std::cref()通常只有在通过泛型代码传递对象时才能正常工作。
     
     比如如果尝试直接输出传递进来的类型为 T 的对象，就会遇到错误，因为std::reference_wrapper<string const>中并没有定义输出运算符：
        template<typename T>
        void printV (T arg) {
        std::cout << arg << ’\n’;
        }
        
        std::string s = "hello";
        printV(s);                      //OK
        printV(std::cref(s));           // ERROR: no operator << for reference wrapper defined
        
    同样下面的代码也会报错，
      因为不能将一个 std::reference_wrapper<string const>对象和一个char const*或者 std::string 进行比较：
        template<typename T1, typename T2>
        bool isless(T1 arg1, T2 arg2)
        {
            return arg1 < arg2;
        }
        
        std::string s = "hello";
        if (isless(std::cref(s), "world"))                  //ERROR
        if (isless(std::cref(s), std::string("world")))     //ERROR
        
    此时即使让 arg1 和 arg2 使用相同的模板参数 T，也不会有帮助：
        template<typename T>
        bool isless(T arg1, T arg2)
        {
            return arg1 < arg2;
        }
    因为编译器在推断 arg1 和 arg2 的类型时会遇到类型冲突。
    
    综上，std::reference_wrapper<>是
        为了让开发者能够像使用“第一类对象（first class object）”一样使用引用，可以对它进行拷贝并将其按值传递给函数模板。
        
    也可以将它用在 class 内部，比如让它持有一个指向容器中对象的引用。
    但是通常总是要将其转换会原始类型。
     
     */
    
    
    namespace case1 {
        void printString(std::string const & s) { std::cout << s << '\n'; }
        
        template <typename T>
        void printT(T arg) {
            printString(arg);               // might convert arg back to std::string
        }
        
        void test_func() {
            std::string s{ "hello" };
            printT(s);                      // print s passed by value
            printT(std::cref(s));           // print s passed by "as if by reference"
        }
        
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_using_std_ref_and_std_cref()
{

    return 0;
}
