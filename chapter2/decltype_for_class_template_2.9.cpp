//
// Created by yangfeng on 2022/7/16.
//
#include "../headers.h"

using namespace std;
/*
 * 直到 C++17，使用类模板时都必须显式指出所有的模板参数的类型（除非它们有默认值）。
 *   从 C++17 开始，这一要求不在那么严格了。如果构造函数能够推断出所有模板参数的类型（对
 *    那些没有默认值的模板参数），就不再需要显式的指明模板参数的类型。
 *  如：
 *    Stack<int> s1;
 *    Stack<int> s2 = s1;  // ok in all versions
 *    Stack s3 = s2; // since c++17
 */

namespace ch2_9 {
    
    template <typename T>
    class Stack {
    private:
        std::vector<T> elems;
        
    public:
        Stack() = default;
        Stack(const T & elem) : elems({elem}) {
        
        }
        
    };
    
    
    /*
     * 类模板对字符串常量参数的类型推断
     *  原则上，可以通过字符串常量来初始化Stack：
     *    Stack stringStack = "hello, world"; // Stack<char const[12]> deduced since c++17
     *  带来的问题：当参数是按照T的引用传递的时候(上面例子就是按照引用传递的)，参数类型不会被decay,
     *    也就是说，一个裸的数组类型，不会被转换成裸指针。
     *    这样我们就等于初始化了一个Stack<char const[12]，类模板中的T都会被实例化成char const[12]，这样
     *    就不能继续向Stack追加一个不同维度的字符串常量了，因为它的类型不是char const[12]。
     *    不过如果参数是按照值传递的，参数类型就会被decay，也就是说裸数组会退化成裸指针。
     *    这样T就会被推断为char const *。实例化的模板类型就会被推断为 Stack<char const *>
     *
     *    基于以上原因，可能有必要将构造函数声明成按值传递参数的形式：
     */
    template <typename T>
    class VStack {
    private:
        std::vector<T> elems;
    public:
        // VStack(T e) : elems({e}) { }
        // move可以避免不必要的拷贝
        [[maybe_unused]]
        explicit VStack(T e) : elems( {std::move(e)} ) { }
    };
    
    VStack(const char *) -> VStack<std::string>;
    
    /* 推断指引 (decuction guides)
     *  针对以上问题，除了将构造函数声明为按值传递的，还有一个解决方案：
     *    由于在容器中处理裸指针容易导致很多问题，对于容器类，
     *    不应该将类型推断为字符的裸指针(char const *)。
     *
     *  可以通过提供“推断指引”来提供额外的模板参数推断规则，或者修正已有的模板参数推断规则。
     *  比如可以定义，当传递一个字符串常量或者c类型的字符串时，
     *  应该用std::string实例化Stack模板类
     *
     *  这个指引语句必须出现在和模板类定义相同的作用域或者命名空间内。
     *  通常它紧跟着模板类的定义。
     *  ->后面的类型被称为推断指引的"guided type"。
     *
     */
    
    
    
    
    

    class Tmp {
    public:
    
    };

}

int
//main()
main_decltype_for_class_template_2_9()
{
    // 通过7初始化这个stack时，模板参数T被推断为int，这样就会实例化出一个Stack<int>
    //ch2_9::Stack intStack = 7;  // since c++17
    
    ch2_9::VStack strStack {"hello, world"};
    
    cout << "end" << endl;

    return 0;
}
