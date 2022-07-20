//
// Created by yangfeng on 2022/7/19.
//
#include "../headers.h"

using namespace std;
/*
 * 除了转发所有参数之外，还可以做些别的事情。比如计算它们的值。
下面的例子先是将参数包中的所有的参数都翻倍，然后将结果传给 print()：
    template<typename… T>
    void printDoubled (T const&… args)
    {
        print (args + args…);
    }
如果这样调用它：
    printDoubled(7.5, std::string("hello"), std::complex<float>(4,2));
效果上和下面的调用相同（除了构造函数方面的不同）：
print(
     7.5 + 7.5,
     std::string("hello") + std::string("hello"),
     std::complex<float>(4,2) + std::complex<float>(4,2);
     );
 */

namespace ch4_4 {
    /*
     * 变参下标
     作为另外一个例子，下面的函数通过一组变参下标来访问第一个参数中相应的元素：
        template<typename C, typename… Idx>
        void printElems (C const& coll, Idx… idx)
        {
            print (coll[idx]…);
        }
    当调用：
        std::vector<std::string> coll = {"good", "times", "say", "bye"};
        printElems(coll,2,0,3);
    时，相当于调用了：
        print (coll[2], coll[0], coll[3]);
    也可以将非类型模板参数声明成参数包。
     
     比如对：
        template<std::size_t… Idx, typename C>
        void printIdx (C const& coll)
        {
            print(coll[Idx]…);
        }
    可以这样调用：
            std::vector<std::string> coll = {"good", "times", "say", "bye"};
            printIdx<2,0,3>(coll);
    效果上和前面的例子相同。
     */

    /*
     *   变参类模板
         类模板也可以是变参的。一个重要的例子是，
         通过任意多个模板参数指定了 class 相应数据成员的类型：
             template<typename… Elements>class Tuple;
             Tuple<int, std::string, char> t; // t can hold integer, string, and character
     */
    
    /*
     * 变参推断指引
    推断指引（参见 2.9 节）也可以是变参的。比如在 C++标准库中，为 std::array 定义了如下推断指引：
    namespace std {
        template<typename T, typename… U> array(T, U…)
        -> array<enable_if_t<(is_same_v<T, U> && …), T>, (1 + sizeof…(U))>;
    }
    
    针对这样的初始化:
        std::array a{42,45,77};
        
    会将指引中的 T 推断为 array（首）元素的类型，而 U...会被推断为剩余元素的类型。
     因此array 中元素总数目是 1 + sizeof...(U)，等效于如下声明:
        std::array<int, 3> a{42,45,77};
     */
    
    class Tmp {
    public:
    
    };

}

int
//main()
main_variadic_class_templates_and_variadic_expressions()
{

    return 0;
}
