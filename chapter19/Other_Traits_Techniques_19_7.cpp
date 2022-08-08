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
        
        /*
        我们同样不能在当前语境中使用如下代码：
            template<typename T>
            using Identity = typename IdentityT<T>::Type;
        我们当然可以定义这样一个别名模板，在其它地方它可能也很有用，但是我们唯独不能将其
        用于 IfThenElse 的定义中，因为任意对 Identity<T>的使用都会立即触发对 IdentityT<T>的完全
        实例化，不然无法获取其 Type 成员。
         */

        template<typename T>
        struct UnsignedT_with_std {
            using Type = typename std::conditional_t<   std::is_integral<T>::value && !std::is_same<T,bool>::value,
                                                        MakeUnsignedT<T>,
                                                        IdentityT<T>
                                                    >::Type;
        };
        
    }
    
    /*
19.7.2 探测不抛出异常的操作
    我们可能偶尔会需要判断某一个操作会不会抛出异常。比如，在可能的情况下，移动构造函
    数应当被标记成 noexcept 的，意思是它不会抛出异常。但是，某一特定 class 的 move
    constructor 是否会抛出异常，通常决定于其成员或者基类的移动构造函数会不会抛出异常。
    比如对于下面这个简单类模板（Pair）的移动构造函数：
    template<typename T1, typename T2>
    class Pair {
    T1 first;
    T2 second;
    public:
    Pair(Pair&& other)
    : first(std::forward<T1>(other.first)),
    second(std::forward<T2>(other.second)) {
    }
    };
    当 T1 或者 T2 的移动操作会抛出异常时，Pair 的移动构造函数也会抛出异常。如果有一个叫
    做 IsNothrowMoveConstructibleT 的萃取，就可以在 Pair 的移动构造函数中通过使用 noexcept
    将这一异常的依赖关系表达出来：
    Pair(Pair&& other)
    noexcept(IsNothrowMoveConstructibleT<T1>::value &&
    IsNothrowMoveConstructibleT<T2>::value)
    : first(std::forward<T1>(other.first)),
    second(std::forward<T2>(other.second))
    五车书馆
    228
    {}
    现在剩下的事情就是去实现 IsNothrowMoveConstructibleT 萃取了。我们可以直接用 noexcept
    运算符实现这一萃取，这样就可以判断一个表达式是否被进行 nothrow 修饰了：
    #include <utility> // for declval
    #include <type_traits> // for bool_constant
    template<typename T>
    struct IsNothrowMoveConstructibleT
    : std::bool_constant<noexcept(T(std::declval<T>()))>
    {};
    这里使用了运算符版本的 noexcept，它会判断一个表达式是否会抛出异常。由于其结果是
    bool 型的，我们可以直接将它用于 std::bool_constant<>基类的定义（std::bool_constant 也被
    用来定义 std::true_type 和 sts::false_type）。
    但是该实现还应该被继续优化，因为它不是 SFINAE 友好的：如果它被一个没有可用移动或
    者拷贝构造函数的类型（这样表达式 T(std::declval<T&&>())就是无效的）实例化，整个程序
    就会遇到问题：
    class E {
    public:
    E(E&&) = delete;
    };
    …
    std::cout
    <<
    IsNothrowMoveConstructibleT<E>::value;
    //
    compiletime
    ERROR
    在这种情况下，我们所期望的并不是让整个程序奔溃，而是获得一个 false 类型的值。
    就像在第 19.4.4 节介绍的那样，在真正做计算之前，必须先对被用来计算结果的表达式的有
    效性进行判断。在这里，我们要在检查移动构造函数是不是 noexcept 之前，先对其有效性
    进行判断。因此，我们要重写之前的萃取实现，给其增加一个默认值是 void 的模板参数，
    并根据移动构造函数是否可用对其进行偏特化：
    #include <utility> // for declval
    #include <type_traits> // for true_type, false_type, and
    bool_constant<>
    // primary template:
    template<typename T, typename = std::void_t<>>
    struct IsNothrowMoveConstructibleT : std::false_type
    { };
    // partial specialization (may be SFINAE’d away):
    template<typename T>
    struct IsNothrowMoveConstructibleT<T,
    std::void_t<decltype(T(std::declval<T>()))>>
    五车书馆
    229
    : std::bool_constant<noexcept(T(std::declval<T>()))>
    {};
    如果在偏特化中对 std::void_t<...>的替换有效，那么就会选择该偏特化实现，在其父类中的
    noexcept(...)表达式也可以被安全的计算出来。否则，偏特化实现会被丢弃（也不会对其进
    行实例化），被实例化的也将是主模板（产生一个 std::false_type 的返回值）。
    值得注意的是，除非真正能够调用移动构造函数，否则我们无法判断移动构造函数是不是会
    抛出异常。也就是说，移动构造函数仅仅是 public 和未被标识为 delete 的还不够，还要求对
    应的类型不能是抽象类（但是抽象类的指针或者引用却可以）。因此，该类型萃取被命名伟
    IsNothrowMoveConstructible，而不是 HasNothrowMoveConstructor。对于其它所有的情况，
    我们都需要编译期支持。
    C++标准库提供了与之对应的萃取 std::is_move_constructible<>，在第 D.3.2 节有对其进行介
    绍。
     */
    
    namespace case2 {
        template<typename T1, typename T2>
        class Pair {
            T1 first;
            T2 second;
        public:
            // Pair(Pair&& other) : first(std::forward<T1>(other.first)), second(std::forward<T2>(other.second)) { }
            /*
            当 T1 或者 T2 的移动操作会抛出异常时，Pair 的移动构造函数也会抛出异常。如果有一个叫
                    做 IsNothrowMoveConstructibleT 的萃取，就可以在 Pair 的移动构造函数中通过使用 noexcept
            将这一异常的依赖关系表达出来：
             */
            Pair(Pair&& other) noexcept(IsNothrowMoveConstructibleT<T1>::value && IsNothrowMoveConstructibleT<T2>::value)
                    : first(std::forward<T1>(other.first)),
                      second(std::forward<T2>(other.second)) {}
                      
            template<typename T>
            struct IsNothrowMoveConstructibleT : std::bool_constant<noexcept(T(std::declval<T>()))> {};
            
            /*
            这里使用了运算符版本的 noexcept，它会判断一个表达式是否会抛出异常。由于其结果是
            bool 型的，我们可以直接将它用于 std::bool_constant<>基类的定义（std::bool_constant 也被
            用来定义 std::true_type 和 sts::false_type）。
            但是该实现还应该被继续优化，因为它不是 SFINAE 友好的：如果它被一个没有可用移动或
            者拷贝构造函数的类型（这样表达式 T(std::declval<T&&>())就是无效的）实例化，整个程序
            就会遇到问题：
                class E {
                public:
                    E(E&&) = delete;
                };
                std::cout << IsNothrowMoveConstructibleT<E>::value; // compiletime ERROR
            在这种情况下，我们所期望的并不是让整个程序奔溃，而是获得一个 false 类型的值 。
             */
            
        };
        
        
    }
    
    /*
    就像在第 19.4.4 节介绍的那样，在真正做计算之前，必须先对被用来计算结果的表达式的有
效性进行判断。在这里，我们要在检查移动构造函数是不是 noexcept 之前，先对其有效性
进行判断。因此，我们要重写之前的萃取实现，给其增加一个默认值是 void 的模板参数，
并根据移动构造函数是否可用对其进行偏特化
     */
    
    namespace case3 {
        // primary template:
        template<typename T, typename = std::void_t<>>
        struct IsNothrowMoveConstructibleT : std::false_type { };
        
        // partial specialization (may be SFINAE’d away):
        template<typename T>
        struct IsNothrowMoveConstructibleT< T,
                    std::void_t<decltype(T(std::declval<T>()))>>
                        : std::bool_constant<noexcept(T(std::declval<T>()))> {};
        
        /*
        如果在偏特化中对 std::void_t<...>的替换有效，那么就会选择该偏特化实现，在其父类中的
noexcept(...)表达式也可以被安全的计算出来。否则，偏特化实现会被丢弃（也不会对其进
行实例化），被实例化的也将是主模板（产生一个 std::false_type 的返回值）。
值得注意的是，除非真正能够调用移动构造函数，否则我们无法判断移动构造函数是不是会
抛出异常。也就是说，移动构造函数仅仅是 public 和未被标识为 delete 的还不够，还要求对
应的类型不能是抽象类（但是抽象类的指针或者引用却可以）。因此，该类型萃取被命名伟
IsNothrowMoveConstructible，而不是 HasNothrowMoveConstructor。对于其它所有的情况，
我们都需要编译期支持。
C++标准库提供了与之对应的萃取 std::is_move_constructible<>，在第 D.3.2 节有对其进行介
绍。
         */
    }
    
    /*
19.7.3 萃取的便捷性（Traits Convenience）
    一个关于萃取的普遍不满是它们相对而言有些繁琐，因为对类型萃取的使用通需要提供一
    个::Type 尾缀，而且在依赖上下文中（dependent context），还需要一个 typename 前缀，两
    者几成范式。当同时使用多个类型萃取时，会让代码形式变得很笨拙，就如同在我们的
    operator+例子中一样，如果想正确的对其进行实现，需要确保不会返回 const 或者引用类型：
    template<typename T1, typename T2>
    Array< typename RemoveCVT<typename RemoveReferenceT<typename
    PlusResultT<T1, T2>::Type >::Type >::Type>
    operator+ (Array<T1> const&, Array<T2> const&);
    通过使用别名模板（alias templates）和变量模板（variable templates），可以让对产生类型
    或者数值的萃取的使用变得很方便。但是也需要注意，在某些情况下这一简便方式并不使用，
    我 们 依 然 要 使 用 最 原 始 的 类 模 板 。 我 们 已 经 讨 论 过 一 个 这 一 类 的 例 子
    （MemberPointerToIntT），但是更详细的讨论还在后面。
    别名模板和萃取（Alias Templates and Traits）
    正如在第 2.8 节介绍的那样，别名模板为降低代码繁琐性提供了一种方法。相比于将类型萃
    取表达成一个包含了 Type 类型成员的类模板，我们可以直接使用别名模板。比如，下面的
    三个别名模板封装了之前的三种类型萃取：
    template<typename T>
    using RemoveCV = typename RemoveCVT<T>::Type;
    template<typename T>
    五车书馆
    230
    using RemoveReference = typename RemoveReferenceT<T>::Type;
    template<typename T1, typename T2>
    using PlusResult = typename PlusResultT<T1, T2>::Type;
    有了这些别名模板，我们可以将 operator+的声明简化成：
    template<typename T1, typename T2>
    Array<RemoveCV<RemoveReference<PlusResultT<T1, T2>>>>
    operator+ (Array<T1> const&, Array<T2> const&);
    这一版本的实现明显更简洁，也让人更容易分辨其组成。这一特性使得别名模板非常适用于
    某些类型萃取。
    但是，将别名模板用于类型萃取也有一些缺点：
    1.
    别名模板不能够被进行特化（在第 16.3 节有过提及），但是由于很多编写萃取的技术
    都依赖于特化，别名模板最终可能还是需要被重新导向到类模板。
    2.
    有些萃取是需要由用户进行特化的，比如描述了一个求和运算符是否是可交换的萃取，
    此时在很多使用都用到了别名模板的情况下，对类模板进行特换会很让人困惑。
    3.
    对别名模板的使用最会让该类型被实例化（比如，底层类模板的特化），这样对于给定
    类型我们就很难避免对其进行无意义的实例化（正如在第 19.7.1 节讨论的那样）。
    对最后一点的另外一种表述方式是，别名模板不可以和元函数转发一起使用（参见第 19.3.2
    节）。
    由于将别名模板用于类型萃取既有优点也有缺点，我们建议像我们在本小节以及 C++标准库
    中那样使用它：同时提供根据遵守特定命名管理的类模板（我们选择使用 T 后缀以及 Type
    类型成员）和遵守了稍微不同命名惯例的别名模板（我们丢弃了 T 尾缀），而且让每一个别
    名模板都基于底层的类模板进行定义。这样，在别名模板能够使代码变得更简洁的地方就是
    用别名模板，否则，对于更为高阶的用户就让他们使用类模板。
    注意，由于某些历史原因，C++标准库选择了不同的命名惯例。其类型萃取会包含一个 type
    类型成员，但是不会有特定的后缀（在 C++11 中为某些类型萃取引入了后缀）。从 C++14
    开始，为之引入了相应的别名模板（直接生成 type），该别名模板会有一个_t 后缀，因为
    没有后缀的名字已经被标准化了（参见第 D.1 节）。
    变量模板和萃取（Variable Templates and Traits）
    对于返回数值的萃取需要使用一个::value（或者类似的成员）来生成萃取的结果。在这种情
    况下，constexpr 修饰的变量模板（在第 5.6 节有相关介绍）提供了一种简化代码的方法。
    比如，下面的变量模板封装了在第 19.3.3 节介绍的 IsSameT 萃取和在第 19.5 节介绍的
    IsConvertibleT 萃取：
    template<typename T1, typename T2>
    五车书馆
    231
    constexpr bool IsSame = IsSameT<T1,T2>::value;
    template<typename FROM, typename TO>
    constexpr bool IsConvertible = IsConvertibleT<FROM, TO>::value;
    此时我们可以将这一类代码：
    if (IsSameT<T,int>::value || IsConvertibleT<T,char>::value) …
    简化成：
    if (IsSame<T,int> || IsConvertible<T,char>) …
    同样由于历史原因，C++标准库也采用了不同的命名惯例。产生 result 结果的萃取类模板并
    没有特殊的后缀，而且它们中的一些在 C++11 中就已经被引入进来了。在 C++17 中引入的
    与之对应的变量模板则有一个_v 后缀（参见第 D.1 节）。
     */
    
    namespace case4 {
        /*
        template<typename T>
        using RemoveCV = typename RemoveCVT<T>::Type;
        
        template<typename T>
        using RemoveReference = typename RemoveReferenceT<T>::Type;
        
        template<typename T1, typename T2>
        using PlusResult = typename PlusResultT<T1, T2>::Type;
        
        template<typename T1, typename T2>
        Array<RemoveCV<RemoveReference<PlusResultT<T1, T2>>>>
        operator+ (Array<T1> const&, Array<T2> const&);
         */
        
        /*
        这一版本的实现明显更简洁，也让人更容易分辨其组成。这一特性使得别名模板非常适用于
        某些类型萃取。
        但是，将别名模板用于类型萃取也有一些缺点：
        1.
        别名模板不能够被进行特化（在第 16.3 节有过提及），但是由于很多编写萃取的技术
        都依赖于特化，别名模板最终可能还是需要被重新导向到类模板。
        2.
        有些萃取是需要由用户进行特化的，比如描述了一个求和运算符是否是可交换的萃取，
        此时在很多使用都用到了别名模板的情况下，对类模板进行特换会很让人困惑。
        3.
        对别名模板的使用最会让该类型被实例化（比如，底层类模板的特化），这样对于给定
        类型我们就很难避免对其进行无意义的实例化（正如在第 19.7.1 节讨论的那样）。
        对最后一点的另外一种表述方式是，别名模板不可以和元函数转发一起使用（参见第 19.3.2
        节）。
        由于将别名模板用于类型萃取既有优点也有缺点，我们建议像我们在本小节以及 C++标准库
        中那样使用它：同时提供根据遵守特定命名管理的类模板（我们选择使用 T 后缀以及 Type
        类型成员）和遵守了稍微不同命名惯例的别名模板（我们丢弃了 T 尾缀），而且让每一个别
        名模板都基于底层的类模板进行定义。这样，在别名模板能够使代码变得更简洁的地方就是
        用别名模板，否则，对于更为高阶的用户就让他们使用类模板。
        注意，由于某些历史原因，C++标准库选择了不同的命名惯例。其类型萃取会包含一个 type
        类型成员，但是不会有特定的后缀（在 C++11 中为某些类型萃取引入了后缀）。从 C++14
        开始，为之引入了相应的别名模板（直接生成 type），该别名模板会有一个_t 后缀，因为
        没有后缀的名字已经被标准化了（参见第 D.1 节）。
                 */
    }
    
    namespace case5 {
    
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_Other_Traits_Techniques_19_7()
{

    return 0;
}