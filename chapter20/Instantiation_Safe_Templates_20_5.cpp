//
// Created by yangfeng on 2022/8/19.
//

#include "../headers.h"

using namespace std;

/*
20.5 实例化安全的模板（Instantiation-Safe Templates）
EnableIf 技术的本质是：只有在模板参数满足某些条件的情况下才允许使用某个模板或者某
个偏特化模板。比如，最为高效的 advanceIter()算法会检查迭代器的参数种类是否可以被转
化成 std::random_access_iterator_tag，也就意味着各种各样的随机访问迭代器都适用于该算
法。
如果我们将这一概念发挥到极致，将所有模板用到的模板参数的操作都编码进 EnableIf 的条
件，会怎样呢？这样一个模板的实例化永远都不会失败，因为那些没有提供 EnableIf 所需操
作的模板参数会导致一个推断错误，而不是任由可能会出错的实例化继续进行。我们称这一
类模板为“实例化安全（instantiation-safe ）”的模板，接下来会对其进行简单介绍。
先从一个计算两个数之间的最小值的简单模板 min()开始。我们可能会将其实现成下面这样：
template<typename T>
T const& min(T const& x, T const& y)
{
if (y < x) {
return y;
}
return x;
}
这个模板要求类型为 T 的两个值可以通过<运算符进行比较，并将比较结果转换成 bool 类型
给 if 语句使用。可以检查类型是否支持<操作符，并计算其返回值类型的类型萃取，在形式
上和我们第 19.4.4 节介绍的 SFINAE 友好的 PlusResultT 萃取类似。为了方便，我们此处依然
列出 LessResultT 的实现：
#include <utility> // for declval()
#include <type_traits> // for true_type and false_type
template<typename T1, typename T2>
class HasLess {
template<typename T> struct Identity;
template<typename U1, typename U2>
static std::true_type
test(Identity<decltype(std::declval<U1>() < std::declval<U2>())>*);
template<typename U1, typename U2>
static std::false_type
test(…);
public:
static constexpr bool value = decltype(test<T1, T2> (nullptr))::value;
};
五车书馆
261
template<typename T1, typename T2, bool HasLess>
class LessResultImpl {
public:
using Type = decltype(std::declval<T1>() < std::declval<T2>());
};
template<typename T1, typename T2>
class LessResultImpl<T1, T2, false> {
};
template<typename T1, typename T2>
class LessResultT
: public LessResultImpl<T1, T2, HasLess<T1, T2>::value> {
};
template<typename T1, typename T2>
using LessResult = typename LessResultT<T1, T2>::Type;
现在就可以通过将该萃取和 IsConvertible 一起使用，使 min()变成实例化安全的：
#include "isconvertible.hpp"
#include "lessresult.hpp"
template<typename T>
EnableIf<IsConvertible<LessResult<T const&, T const&>, bool>, T const&>
min(T const& x, T const& y)
{
if (y < x) {
return y;
}
return x;
}
通过各种实现了不同<运算符的类型来调用 min()，要更能说明问题一些，就像下面这样：
#include"min.hpp"
struct X1 { };
bool operator< (X1 const&, X1 const&) { return true; }
struct X2 { };
bool operator<(X2, X2) { return true; }
struct X3 { };
bool operator<(X3&, X3&) { return true; }
struct X4 { };
五车书馆
262
struct BoolConvertible {
operator bool() const { return true; } // implicit conversion to bool
};
struct X5 { };
BoolConvertible operator< (X5 const&, X5 const&)
{
return BoolConvertible();
}
struct NotBoolConvertible { // no conversion to bool
};
struct X6 { };
NotBoolConvertible operator< (X6 const&, X6 const&)
{
return NotBoolConvertible();
}
struct BoolLike {
explicit operator bool() const { return true; } // explicit conversion to bool
};
struct X7 { };
BoolLike operator< (X7 const&, X7 const&) { return BoolLike(); }
int main()
{
min(X1(), X1()); // X1 can be passed to min()
min(X2(), X2()); // X2 can be passed to min()
min(X3(), X3()); // ERROR: X3 cannot be passed to min()
min(X4(), X4()); // ERROR: X4 cannot be passed to min()
min(X5(), X5()); // X5 can be passed to min()
min(X6(), X6()); // ERROR: X6 cannot be passed to min()
min(X7(), X7()); // UNEXPECTED ERROR: X7 cannot be passed to min()
}
在编译上述程序的时候，要注意虽然针对 min()函数会报出 4 个错误（X3，X4，X6，以及 X7），
但它们都不是从 min()的函数体中报出来的（如果不是实例化安全的话，则会从函数体中报
出错误）。相反，编译器只会抱怨说没有合适的 min()函数，因为唯一的选择已经被 SFINAE
五车书馆
263
排除了。Clang 会报出如下错误：
min.cpp:41:3: error: no matching function for call to ’min’
min(X3(), X3()); // ERROR: X3 cannot be passed to min
^~~
./min.hpp:8:1: note: candidate template ignored: substitution
failure
[with T = X3]: no type named ’Type’ in
’LessResultT<const X3 &, const X3 &>’
min(T const& x, T const& y)
g++报出的部分错误信息如下：
min.cpp: In function 'int main()':
min.cpp:83:19: error: no matching function for call to 'min(X3, X3)'
min(X3(), X3()); // ERROR: X3 cannot be passed to min()
^
min.cpp:72:1: note: candidate: template<class T>
std::enable_if_t<std::is_convertible<typename LessResultT<const T&, const
T&>::Type, bool>::value, const T&> min(const T&, const T&)
min(T const& x, T const& y)
^
min.cpp:72:1: note:
template argument deduction/substitution failed:
min.cpp: In substitution of 'template<class T>
std::enable_if_t<std::is_convertible<typename LessResultT<const T&, const
T&>::Type, bool>::value, const T&> min(const T&, const T&) [with T = X3]':
min.cpp:83:19:
required from here
min.cpp:72:1: error: no type named 'Type' in 'class LessResultT<const X3&, const X3&>'
因此可以看出，EnableIf 只允许针对那些满足了模板要求的类型（X1，X2，和 X5）进行实例
化，也就永远不会从 min()的函数体中报出错误。
例子中的最后一个类型（X7），体现了实现实例化安全模板过程中的一些很微妙的地方。如
果 X7 是被传递给非实例化安全的 min()，那么可以成功实例化。但是对于实例化安全的 min()，
实例化却会失败，因为 BoolLike 不可以被隐式的转换成 bool 类型。这里的区别很微妙：在
某些情况下，显式的向 bool 的转换可以被隐式的使用，比如控制语句（if，while，for 以及
do）的布尔型条件，内置的！，&&以及||运算符，还有三元运算符?:。在这些情况下，该
值被认为是“语境上可以转换成 bool”
但是，我们对一般的、可以隐式地向 bool 转换这一条件的坚持，导致实例化安全的模板被
过分限制了；也就是说，在 EnableIf 中指定的条件要比我们实际需要的条件更为严格（正确
实例化模板所需要的条件）。另一方面，如果我们完全忘记了可以向 bool 转换这一要求，
那么对于 min()模板的要求就过于宽松了，这样的话对于某些类型可能会遇到实例化错误（比
如 X6）。
为了解决 min()中这一由实例化安全带来的问题，我们需要一个可以判断某个类型是否是“语
五车书馆
264
境上可以转换成 bool”的萃取技术。控制流程语句对该萃取技术的实现没有帮助，因为语
句不可以出现在 SFINAE 上下文中，同样的，可以被任意类型重载的逻辑操作也不可以。幸
运的是，三元运算符?:是一个表达式，而且不可以被重载，因此它可以被用来测试一个类型
是否是“语境上可以转换成 bool”的：
#include <utility> // for declval()
#include <type_traits> // for true_type and false_type
template<typename T>
class IsContextualBoolT {
private:
template<typename T> struct Identity;
template<typename U>
static std::true_type test(Identity<decltype(declval<U>()? 0 : 1)>*);
template<typename U>
static std::false_type test(…);
public:
static constexpr bool value = decltype(test<T> (nullptr))::value;
};
template<typename T>
constexpr bool IsContextualBool = IsContextualBoolT<T>::value;
有了这一萃取，我们就可以实现一个使用了正确的 EnableIf 条件且实例化安全的 min()了:
#include "iscontextualbool.hpp"
#include "lessresult.hpp"
template<typename T>
EnableIf<IsContextualBool<LessResult<T const&, T const&>>, T const&>
min(T const& x, T const& y)
{
if (y < x) {
return y;
}
return x;
}
将各种各样的条件检查，组合进描述了类型种类（比如前向迭代器）的萃取技术，并将这些
萃取技术一起放在 EnableIf 的条件检查中，这一使 min()变得实例化安全的技术可以被推广
到用于描述其它重要模板的条件。这样做一方面可以获得更好的重载行为，另一方面也可以
避免在实例化深层次嵌套的模板时，编译器遇到错误后会产生过于冗长的错误信息的问题。
但是此时的类错误信息通常不会指出具体是哪一个操作出了错误。而且，正如我们在 min()
中展现的那样，准确的判断并编码相关的条件可能是让人抓狂的。我们在第 28.2 节探讨了
使用了这些萃取的 debug 技术。
 */


namespace ch20_5 {

    /*
     
     */
    namespace case1 {
        /*
        template<typename T>
        T const& min(T const& x, T const& y)
        {
            if (y < x) {
                return y;
            }
            return x;
        }
         */
        template<typename T1, typename T2>
        class HasLess {
            template<typename T>
            struct Identity;
            
            template<typename U1, typename U2>
            static std::true_type test(Identity<decltype(std::declval<U1>() < std::declval<U2>())>*);
            
            template<typename U1, typename U2>
            static std::false_type test(...);
            
        public:
            static constexpr bool value = decltype(test<T1, T2> (nullptr))::value;
        };
        
        template<typename T1, typename T2, bool HasLess>
        class LessResultImpl {
        public:
            using Type = decltype(std::declval<T1>() < std::declval<T2>());
        };
        
        template<typename T1, typename T2>
        class LessResultImpl<T1, T2, false> {
        };
        
        template<typename T1, typename T2>
        class LessResultT
                : public LessResultImpl<T1, T2, HasLess<T1, T2>::value> {
        };
        
        template <bool cond, typename = void>
        struct EnableIfT {
            // using Type = void;
        };
    
        template <typename T>
        struct EnableIfT<true, T> {
            using Type = T;
        };
        
        template <bool cond, typename T>
        using EnableIf = typename EnableIfT<cond, T>::Type;
        
        
        template<typename T1, typename T2>
        using LessResult = typename LessResultT<T1, T2>::Type;
        
        template <typename T, typename U>
        struct IsConvertibleT : public false_type {
            static constexpr bool value = true;
        };
        
        template <typename T, typename U>
        constexpr bool IsConvertible = IsConvertibleT<T, U>::value;
        
    
        template<typename T>
        EnableIf<IsConvertible<LessResult<T const&, T const&>, bool>, T const&>
        min(T const& x, T const& y)
        {
            if (y < x)
                return y;
            return x;
        }
    
        struct X1 { };
        bool operator< (X1 const&, X1 const&) { return true; }
        
        struct X2 { };
        bool operator<(X2, X2) { return true; }
        
        struct X3 { };
        bool operator<(X3&, X3&) { return true; }
        
        struct X4 { };
        
        struct BoolConvertible {
            operator bool() const { return true; } // implicit conversion to bool
        };
        
        struct X5 { };
        
        BoolConvertible operator< (X5 const&, X5 const&)
        {
            return BoolConvertible();
        }
        
        struct NotBoolConvertible { // no conversion to bool
        };
        
        struct X6 { };
        
        NotBoolConvertible operator< (X6 const&, X6 const&)
        {
            return NotBoolConvertible();
        }
        
        struct BoolLike {
            explicit operator bool() const { return true; } // explicit conversion to bool
        };
        
        struct X7 { };
        BoolLike operator< (X7 const&, X7 const&) {
            cout << "BoolLike" << endl;
            return BoolLike();
        }
        
        void test()
        {
            min(X1(), X1()); // X1 can be passed to min()
            min(X2(), X2()); // X2 can be passed to min()
            // min(X3(), X3()); // ERROR: X3 cannot be passed to min()
            // min(X4(), X4()); // ERROR: X4 cannot be passed to min()
            min(X5(), X5()); // X5 can be passed to min()
            // min(X6(), X6()); // ERROR: X6 cannot be passed to min()
            min(X7(), X7()); // UNEXPECTED ERROR: X7 cannot be passed to min()
        }
        
        
    }
    
    namespace case2 {
        
        template<typename T>
        class IsContextualBoolT {
        private:
            template<typename U> struct Identity;
            template<typename U>
            static std::true_type test(Identity<decltype(declval<U>()? 0 : 1)>*);
            template<typename U>
            static std::false_type test(...);
            
        public:
            static constexpr bool value = decltype(test<T> (nullptr))::value;
        };
        
        template<typename T>
        constexpr bool IsContextualBool = IsContextualBoolT<T>::value;
    
        template<typename T>
        case1::EnableIf<IsContextualBool<case1::LessResult<T const&, T const&>>, T const&>
        min(T const& x, T const& y)
        {
            if (y < x) {
                return y;
            }
            return x;
        }
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
main()
//main_ch20_5()
{
    ch20_5::case1::test();

    return 0;
}
