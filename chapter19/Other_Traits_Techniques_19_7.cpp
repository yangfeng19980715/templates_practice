//
// Created by yangfeng on 2022/8/6.
//

#include "../headers.h"

/*
19.7 其它的萃取技术
    最后让我们来介绍其它一些在定义萃取时可能会用到的方法。
 */

using namespace std;

namespace ch19_7 {
    
    /*
19.7.1 If-Then-Else
    在上一小节中，PlusResultT 的定义采用了和之前完全不同的实现方法，该实现方法依赖于另
    一个萃取（HasPlusT）的结果。我们可以用一个特殊的类型模板 IfThenElse 来表达这一
    if-then-else 的行为，它接受一个 bool 型的模板参数，并根据该参数从另外两个类型参数中
    间做选择：
    #ifndef IFTHENELSE_HPP
    #define IFTHENELSE_HPP
    // primary template: yield the second argument by default and rely on
    // a partial specialization to yield the third argument
    // if COND is false
    template<bool COND, typename TrueType, typename FalseType>
    struct IfThenElseT {
    using Type = TrueType;
    };
    // partial specialization: false yields third argument
    template<typename TrueType, typename FalseType>
    struct IfThenElseT<false, TrueType, FalseType> {
    using Type = FalseType;
    };
    template<bool COND, typename TrueType, typename FalseType>
    using IfThenElse = typename IfThenElseT<COND, TrueType,
    FalseType>::Type;
    #endif //IFTHENELSE_HPP
    五车书馆
    225
    下面的例子展现了该模板的一种应用，它定义了一个可以为给定数值选择最合适的整形类型
    的函数：
    #include <limits>
    #include "ifthenelse.hpp"
    template<auto N>
    struct SmallestIntT {
    using Type =
    typename IfThenElseT<N <= std::numeric_limits<char> ::max(), char,
    typename IfThenElseT<N <=
    std::numeric_limits<short> ::max(), short,
    typename IfThenElseT<N <=
    std::numeric_limits<int> ::max(), int,
    typename IfThenElseT<N <=
    std::numeric_limits<long>::max(), long,
    typename IfThenElseT<N <=
    std::numeric_limits<long long>::max(), long long, //then
    void //fallback
    >::Type
    >::Type
    >::Type
    >::Type
    >::Type;
    };
    需要注意的是，和常规的 C++ if-then-else 语句不同，在最终做选择之前，then 和 else 分支
    中的模板参数都会被计算，因此两个分支中的代码都不能有问题，否则整个程序就会有问题。
    考虑下面这个例子，一个可以为给定的有符号类型生成与之对应的无符号类型的萃取。已经
    有一个标准萃取（std::make_unsigned）可以做这件事情，但是它要求传递进来的类型是有
    符号的整形，而且不能是 bool 类型；否则它将使用未定义行为的结果（参见第 D.4 节）。
    这一萃取不够安全，因此最好能够实现一个这样的萃取，当可能的时候，它就正常返回相应
    的无符号类型，否则就原样返回被传递进来的类型（这样，当传递进来的类型不合适时，也
    能避免触发未定义行为）。下面这个简单的实现是不行的：
    // ERROR: undefined behavior if T is bool or no integral type:
    template<typename T>
    struct UnsignedT {
    using Type = IfThenElse<std::is_integral<T>::value
    && !std::is_same<T,bool>::value, typename std::make_unsigned<T>::type,
    T>;
    };
    因为在实例化 UnsingedT<bool>的时候，行为依然是未定义的，编译期依然会试图从下面的
    代码中生成返回类型：
    typename std::make_unsigned<T>::type
    五车书馆
    226
    为了解决这一问题，我们需要再引入一层额外的间接层，从而让 IfThenElse 的参数本身用类
    型函数去封装结果：
    // yield T when using member Type:
    template<typename T>
    struct IdentityT {
    using Type = T;
    };
    // to make unsigned after IfThenElse was evaluated:
    template<typename T>
    struct MakeUnsignedT {
    using Type = typename std::make_unsigned<T>::type;
    };
    template<typename T>
    struct UnsignedT {
    using Type = typename IfThenElse<std::is_integral<T>::value
    && !std::is_same<T,bool>::value,
    MakeUnsignedT<T>,
    IdentityT<T>
    >::Type;
    };
    在这一版 UnsignedT 的定义中，IfThenElse 的类型参数本身也都是类型函数的实例。只不过
    在最终 IfThenElse 做出选择之前，类型函数不会真正被计算。而是由 IfThenElse 选择合适的
    类型实例（MakeUnsignedT 或者 IdentityT）。最后由::Type 对被选择的类型函数实例进行计
    算，并生成结果 Type。
    此处值得强调的是，之所以能够这样做，是因为 IfThenElse 中未被选择的封装类型永远不会
    被完全实例化。下面的代码也不能正常工作：
    template<typename T>
    struct UnsignedT {
    using Type = typename IfThenElse<std::is_integral<T>::value
    && !std::is_same<T,bool>::value,
    MakeUnsignedT<T>::Type,
    T
    >::Type;
    };
    我们必须要延后对 MakeUnsignedT<T>使用::Type，也就是意味着，我们同样需要为 else 分支
    中的 T 引入 IdentyT 辅助模板，并同样延后对其使用::Type。
    我们同样不能在当前语境中使用如下代码：
    五车书馆
    227
    template<typename T>
    using Identity = typename IdentityT<T>::Type;
    我们当然可以定义这样一个别名模板，在其它地方它可能也很有用，但是我们唯独不能将其
    用于 IfThenElse 的定义中，因为任意对 Identity<T>的使用都会立即触发对 IdentityT<T>的完全
    实例化，不然无法获取其 Type 成员。
    在 C++标准库中有与 IfThenElseT 模板对应的模板（std::conditional<>，参见第 D.5 节）。使
    用这一标准库模板实现的 UnsignedT 萃取如下：
    template<typename T>
    struct UnsignedT {
    using Type = typename std::conditional_t<std::is_integral<T>::value
    && !std::is_same<T,bool>::value,
    MakeUnsignedT<T>,
    IdentityT<T>
    >::Type;
    };
     */

    namespace case1 {
        // primary template: yield the second argument by default and rely on a partial specialization to yield the third argument
        // if COND is false
        template<bool COND, typename TrueType, typename FalseType>
        struct IfThenElseT {
            using Type = TrueType;
        };
        
        // partial specialization: false yields third argument
        template<typename TrueType, typename FalseType>
        struct IfThenElseT<false, TrueType, FalseType> {
            using Type = FalseType;
        };
        
        template<bool COND, typename TrueType, typename FalseType>
        using IfThenElse = typename IfThenElseT<COND, TrueType, FalseType>::Type;
    
        template<auto N>
        struct SmallestIntT {
            using Type =
                    typename IfThenElseT<N <= std::numeric_limits<char> ::max(), char,
                            typename IfThenElseT<N <=
                                                 std::numeric_limits<short> ::max(), short,
                                    typename IfThenElseT<N <=
                                                         std::numeric_limits<int> ::max(), int,
                                            typename IfThenElseT<N <=
                                                                 std::numeric_limits<long>::max(), long,
                                                    typename IfThenElseT<N <=
                                                                         std::numeric_limits<long long>::max(), long long, //then
                                                            void //fallback
                                                    >::Type
                                            >::Type
                                    >::Type
                            >::Type
                    >::Type;
        };
    
        /*
     需要注意的是，和常规的 C++ if-then-else 语句不同，在最终做选择之前，then 和 else 分支
        中的模板参数都会被计算，因此两个分支中的代码都不能有问题，否则整个程序就会有问题。
        考虑下面这个例子，一个可以为给定的有符号类型生成与之对应的无符号类型的萃取。已经
        有一个标准萃取（std::make_unsigned）可以做这件事情，但是它要求传递进来的类型是有
        符号的整形，而且不能是 bool 类型；否则它将使用未定义行为的结果（参见第 D.4 节）。
        这一萃取不够安全，因此最好能够实现一个这样的萃取，当可能的时候，它就正常返回相应
        的无符号类型，否则就原样返回被传递进来的类型（这样，当传递进来的类型不合适时，也
        能避免触发未定义行为）。下面这个简单的实现是不行的, 因为在实例化 UnsingedT<bool>的时候，行为依然是未定义的。
         */
        // ERROR: undefined behavior if T is bool or no integral type:
        template<typename T>
        struct UnsignedT_with_UB {
            using Type = IfThenElse<    std::is_integral<T>::value && !std::is_same<T,bool>::value,
                                        typename std::make_unsigned<T>::type,
                                        T
                                    >;
        };
        
        /*
        为了解决这一问题，我们需要再引入一层额外的间接层，从而让 IfThenElse 的参数本身用类型函数去封装结果。
         在这一版 UnsignedT 的定义中，IfThenElse 的类型参数本身也都是类型函数的实例。只不过
        在最终 IfThenElse 做出选择之前，类型函数不会真正被计算。而是由 IfThenElse 选择合适的
        类型实例（MakeUnsignedT 或者 IdentityT）。最后由::Type 对被选择的类型函数实例进行计
        算，并生成结果 Type。
        此处值得强调的是，之所以能够这样做，是因为 IfThenElse 中未被选择的封装类型永远不会
        被完全实例化。
         我们必须要延后对 MakeUnsignedT<T>使用::Type，也就是意味着，我们同样需要为 else 分支
中的 T 引入 IdentyT 辅助模板，并同样延后对其使用::Type。
         */
        
        // yield T when using member Type:
        template<typename T>
        struct IdentityT {
            using Type = T;
        };
        
        // to make unsigned after IfThenElse was evaluated:
        template<typename T>
        struct MakeUnsignedT {
            using Type = typename std::make_unsigned<T>::type;
        };
        
        template<typename T>
        struct UnsignedT {
            using Type = typename IfThenElse<std::is_integral<T>::value && !std::is_same<T,bool>::value,
                                                MakeUnsignedT<T>,
                                                IdentityT<T>
                                            >::Type;
        };
        
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
main()
//main_Other_Traits_Techniques_19_7()
{

    return 0;
}