//
// Created by yangfeng on 2022/7/21.
//
#include "../headers.h"

using namespace std;

/*
    通过 std::enable_if<>禁用模板
        从 C++11 开始，通过 C++标准库提供的辅助模板 std::enable_if<>，可以在某些编译期条件下忽略掉函数模板。
        
        比如，如果函数模板 foo<>的定义如下：
            template<typename T>
            typename std::enable_if<(sizeof(T) > 4)>::type
            foo() {
            }
        这一模板定义会在 sizeof(T) > 4 不成立的时候被忽略掉。如果 sizeof<T> > 4 成立，函数模板
        会展开成：
            template<typename T>
            void foo() {
            }
        
        也就是说 std::enable_if<>是一种类型萃取（type trait），
            它会根据一个作为其（第一个）模板参数的编译期表达式决定其行为：
            
         如果这个表达式结果为 true，它的 type 成员会返回一个类型：
            -- 如果没有第二个模板参数，返回类型是 void。
            -- 否则，返回类型是其第二个参数的类型。
            
         如果表达式结果 false，则其成员类型是未定义的。
            根据模板的一个叫做 SFINAE（substitute failure is not an error，替换失败不是错误，将在 8.4 节进行介绍）的规则，
                这会导致包含 std::enable_if<>表达式的函数模板被忽略掉。
            
        由于从 C++14 开始所有的模板萃取（type traits）都返回一个类型，
            因此可以使用一个与之对应的别名模板 std::enable_if_t<>，这样就可以省略掉 template 和::type 了。
*/

namespace ch6_3 {
    
    namespace case1 {
        template <typename T>
        typename std::enable_if<(sizeof(T) > 4), T>::type foo() {
            cout << "test enable_if<>" << endl;
            return T{};
        }
    }
    
/*
    但是由于将 enable_if 表达式放在声明的中间不是一个明智的做法，
        因此使用 std::enable_if<>的更常见的方法是使用一个额外的、有默认值的模板参数：
            template<typename T, typename = std::enable_if_t<(sizeof(T) > 4)>>
            void foo() {
            }
    如果 sizeof(T) > 4，它会被展开成：
            template<typename T, typename = void>
            void foo() {
            }
            
    如果你认为这依然不够明智，并且希望模板的约束更加明显，
        那么你可以用别名模板（alias template）给它定义一个别名：
            template<typename T>
            using EnableIfSizeGreater4 = std::enable_if_t<(sizeof(T) > 4)>;
            
            template<typename T, typename = EnableIfSizeGreater4<T>>
            void foo() {
            }
*/

    class Tmp {
    public:
    
    };

}

int
//main()
main_disable_templates_with_enable_if()
{
    ch6_3::case1::foo<long long>();

    return 0;
}
