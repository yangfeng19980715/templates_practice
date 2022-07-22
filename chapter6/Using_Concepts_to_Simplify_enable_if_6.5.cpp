//
// Created by yangfeng on 2022/7/21.
//
#include "../headers.h"

using namespace std;

/*
使用 concept 简化 enable_if<>表达式
    即使使用了模板别名，enable_if 的语法依然显得很蠢，
      因为它使用了一个变通方法：为了达到目的，使用了一个额外的模板参数，并且通过“滥用”这个参数对模板的使用做了限制。
      
    这样的代码不容易读懂，也使模板中剩余的代码不易理解。
    原则上我们所需要的只是一个能够对函数施加限制的语言特性，
        当这一限制不被满足的时候，函数会被忽略掉。
        
    这个语言特性就是人们期盼已久的 concept，可以通过其简单的语法对函数模板施加限制条件。
    
    不幸的是，虽然已经讨论了很久，但是 concept 依然没有被纳入 C++17 标准。
    一些编译器目前对 concept 提供了试验性的支持，
        不过其很有可能在 C++17 之后的标准中得到支持（目前确定将在 C++20 中得到支持）。
        
    通过使用 concept 可以写出下面这样的代码：
        template<typename STR>
        requires std::is_convertible_v<STR,std::string>
        Person(STR&& n) : name(std::forward<STR>(n)) {
            …
        }
        
    甚至可以将其中模板的使用条件定义成通用的 concept：
        template<typename T>
        concept ConvertibleToString = std::is_convertible_v<T,std::string>;
        
    然后将这个 concept 用作模板条件：
        template<typename STR>
        requires ConvertibleToString<STR>
        Person(STR&& n) : name(std::forward<STR>(n)) {
            …
        }
        
    也可以写成下面这样：
        template<ConvertibleToString STR>
        Person(STR&& n) : name(std::forward<STR>(n)) {
            …
        }
*/

namespace ch6_5 {
    namespace case1 {
        // 定义转换的通用concept
        template <typename T>
        concept ConvertibleToString = std::is_convertible_v<T, std::string>;
        
        class Person {
        private:
            std::string name;
        public:
            
            
            /*
             // 不使用通用concept的方法
            template <typename STR>
            requires std::is_convertible_v<STR, std::string>
            Person(STR&& n) : name(std::forward<STR>(n)) {
                cout << "requires std::is_comvertible_v<STR, std::string>" << endl;
            }
             */
            
            /*
            // 将通用concept用作模板条件
            template <typename STR>
            requires ConvertibleToString<STR>
            Person(STR && n) : name(std::forward<STR>(n)) {
                cout << "requires std::is_comvertible_v<STR, std::string>" << endl;
            }
             */
            
            // 也可以写成这样
            template<ConvertibleToString STR>
            Person(STR && n) : name(std::forward<STR>(n)) {
                cout << "requires std::is_comvertible_v<STR, std::string>" << endl;
            }
            
        
        };
        
        
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_using_concepts_to_simplify_enable_if()
{
    ch6_5::case1::Person p { "hello" };

    return 0;
}
