//
// Created by yangfeng on 2022/8/3.
//

#include "../headers.h"

using namespace std;

/*
基于 SFINAE 的萃取（SFINAE-Based Traits）
    SFINAE（参见第 8.4 节和第 15.7 节）会将在模板参数推断过程中，
      构造无效类型和表达式的潜在错误（会导致程序出现语法错误）转换成简单的推断错误，
      这样就允许重载解析继续在其它待选项中间做选择。
      
      虽然 SFINAE 最开始是被用来避免与函数模板重载相关的伪错误，我们也可以用它在编译期间判断特定类型和表达式的有效性。
      比如我们可以通过萃取来判断一个类型是否有某个特定的成员，是否支持某个特定的操作，或者该类型本身是不是一个类。
      
    基于 SFINAE 的两个主要技术是：用 SFINAE 排除某些重载函数，以及用 SFINAE 排除某些偏特化。
 */

namespace ch19_4 {
    /*
10.4.1 用 SFINAE 排除某些重载函数
    我们触及到的第一个基于 SFINAE 的例子是将 SFINAE 用于函数重载，以判断一个类型是否是
    默认可构造的，对于可以默认构造的类型，就可以不通过值初始化来创建对象。也就是说，
    对于类型 T，诸如 T()的表达式必须是有效的。
    一个基础的实现可能会像下面这样：
    #include "issame.hpp"
    template<typename T>
    struct IsDefaultConstructibleT {
    private:
    // test() trying substitute call of a default constructor for
    //T passed as U :
    template<typename U, typename = decltype(U())>
    static char test(void*);// test() fallback:
    template<typename>
    static long test(...);
    public:
    static constexpr bool value =
    IsSameT<decltype(test<T>(nullptr)), char>::value;
    };
    通过函数重载实现一个基于 SFINAE 的萃取的常规方式是声明两个返回值类型不同的同名
    （test()）重载函数模板：
    template<...> static char test(void*);
    template<...> static long test(...);
    第一个重载函数只有在所需的检查成功时才会被匹配到（后文会讨论其实现方式）。第二个
    重载函数是用来应急的：它会匹配任意调用，但是由于它是通过”...”（省略号）进行匹配的，
    因此其它任何匹配的优先级都比它高（参见附录 C.2）。
    返回值 value 的具体值取决于最终选择了哪一个 test 函数：
    static constexpr bool value
    = IsSameT<decltype(test<...>(nullptr)), char>::value;
    如果选择的是第一个 test()函数，由于其返回值类型是 char，value 会被初始化伟 isSame<char,
    char>，也就是 true。否则，value 会被初始化伟 isSame<long, char>，也就是 false。
    现在，到了该处理我们所需要检测的属性的时候了。目标是只有当我们所关心的测试条件被
    满足的时候，才可以使第一个 test()有效。在这个例子中，我们想要测试的条件是被传递进
    来的类型 T 是否是可以被默认构造的。为了实现这一目的，我们将 T 传递给 U，并给第一个
    test()声明增加一个无名的（dummy）模板参数，该模板参数被一个只有在这一转换有效的
    情况下才有效的构造函数进行初始化。在这个例子中，我们使用的是只有当存在隐式或者显
    式的默认构造函数 U()时才有效的表达式。我们对 U()的结果施加了 deltype 操作，这样就可
    以用其结果初始化一个类型参数了。
    五车书馆
    203
    第二个模板参数不可以被推断，因为我们不会为之传递任何参数。而且我们也不会为之提供
    显式的模板参数。因此，它只会被替换，如果替换失败，基于 SFINAE，相应的 test()声明会
    被丢弃掉，因此也就只有应急方案可以匹配相应的调用。
    因此，我们可以像下面这样使用这一萃取：
    IsDefaultConstructibleT<int>::value //yields true
    struct S {
    S() = delete;
    };
    IsDefaultConstructibleT<S>::value //yields false
    但是需要注意，我们不能在第一个 test()声明里直接使用模板参数 T：
    template<typename T>
    struct IsDefaultConstructibleT {
    private:
    // ERROR: test() uses T directly:
    template<typename, typename = decltype(T())>
    static char test(void*);
    // test() fallback:
    template<typename>
    static long test(...);
    public:
    static constexpr bool value
    = IsSameT<decltype(test<T>(nullptr)), char>::value;
    };
    但是这样做并不可以，因为对于任意的 T，所有模板参数为 T 的成员函数都会被执行模板参
    数替换，因此对一个不可以默认构造的类型，这些代码会遇到编译错误，而不是忽略掉第一
    个 test()。通过将类模板的模板参数 T 传递给函数模板的参数 U，我们就只为第二个 test()的
    重载创建了特定的 SFINAE 上下文。
    另一种基于 SFINAE 的萃取的实现策略
    远在 1998 年发布第一版 C++标准之前，基于 SFINAE 的萃取的实现就已经成为了可能。该方
    法的核心一致都是实现两个返回值类型不同的重载函数模板：
    template<...> static char test(void*);
    template<...> static long test(...);
    但是，在最早的实现技术中，会基于返回值类型的大小来判断使用了哪一个重载函数（也会
    用到 0 和 enum，因为在当时 nullptr 和 constexpr 还没有被引入）：
    五车书馆
    204
    enum { value = sizeof(test<...>(0)) == 1 };
    在某些平台上，sizeof(char)的值可能会等于 sizeof(long)的值。比如，在数字信号处理器（digital
    signal processors，DSP）或者旧的 Cray 机器上，所有内部的基础类型的大小都可以相同。比
    如根据定义 sizeof(char) == 1，但是在这些机器上，sizeof(long)，甚至是 sizeof(long long)的值
    也都是 1。
    基于此，我们希望能够确保 test()的返回值类型在所有的平台上都有不同的值。比如，在定
    义了：
    using Size1T = char;
    using Size2T = struct { char a[2]; };
    或者：
    using Size1T = char(&)[1];
    using Size2T = char(&)[2];
    之后，可以像下面这样定义 test()的两个重载版本：
    template<...> static Size1T test(void*); // checking test()
    template<...> static Size2T test(...); // fallback
    这样，我们要么返回 Size1T，其大小为 1，要么返回 Size2T，在所有的平台上其值都至少是
    2。
    使用了上述某一种方式的代码目前依然很常见。
    但是要注意，传递给 test()的调用参数的类型并不重要。我们所要保证的是被传递的参数和
    所期望的类型能够匹配。比如，可以将其定义成能够接受整型常量 42 的形式：
    template<...> static Size1T test(int); // checking test()
    template<...> static Size2T test(...); // fallback
    ...
    enum { value = sizeof(test<...>(42)) == 1 };
    将基于 SFINAE 的萃取变参预测型萃取
    正如在第 19.3.3 节介绍的那样，返回 bool 值的萃取，应该返回一个继承自 std::true_type 或
    者 std::false_type 的值。使用这一方式，同样可以解决在某些平台上 sizeof(char) == sizeof(long)
    的问题。
    为了这一目的，我们需要间接定义一个 IsDefaultConstructibleT。该萃取本身需要继承自一个
    辅助类的 Type 成员，该辅助类会返回所需的基类。幸运的是，我们可以简单地将 test()的返
    回值类型用作对应的基类：
    template<...> static std::true_type test(void*); // checking test()
    五车书馆
    205
    template<...> static std::false_type test(...); // fallback
    然后可以将基类的 Type 成员声明为：
    using Type = decltype(test<FROM>(nullptr));
    此时我们也不再需要使用 IsSameT 萃取了。
    优化之后，完整的 IsDefaultConstructibleT 的实现如下：
    #include <type_traits>
    template<typename T>
    struct IsDefaultConstructibleHelper {
    private:
    // test() trying substitute call of a default constructor for
    T passed as U:
    template<typename U, typename = decltype(U())>
    static std::true_type test(void*);
    // test() fallback:
    template<typename>
    static std::false_type test(...);
    public:
    using Type = decltype(test<T>(nullptr));
    };
    template<typename T>
    struct IsDefaultConstructibleT :
    IsDefaultConstructibleHelper<T>::Type {
    };
    现在，如果第一个 test()函数模板是有效的话，那么它就将是被选择的重载函数，因此成员
    IsDefaultConstructibleHelper::Type 会 被 其 返 回 值 类 型 std::true_type 初 始 化 。 这 样 的 话
    IsConvertibleT<>就会继承自 std::true_type。
    如 果 第 一 个 test() 函 数 模 板 是 无 效 的 话 ， 那 么 它 就 会 被 SFINAE 剔 除 掉 ，
    IsDefaultConstructibleHelper::Type 也 就 会 被 应 急 test() 的 返 回 值 类 型 初 始 化 ， 也 就 是
    std::false_type。这样的话 IsConvertibleT<>就会继承自 std::false_type。
     */

    namespace case1 {
        /*
    我们触及到的第一个基于 SFINAE 的例子是将 SFINAE 用于函数重载，以判断一个类型是否是
        默认可构造的，对于可以默认构造的类型，就可以不通过值初始化来创建对象。也就是说，
        对于类型 T，诸如 T()的表达式必须是有效的。
        一个基础的实现可能会像下面这样：
         */
        template<typename T>
        struct IsDefaultConstructibleT {
        private:
            /*
            通过函数重载实现一个基于 SFINAE 的萃取的常规方式是声明两个返回值类型不同的同名（test()）重载函数模板：
             */
            // test() trying substitute call of a default constructor for T passed as U :
            template<typename U, typename = decltype(U())>
            static char test(void*);                                // test() fallback:
            
            template<typename>
            static long test(...);
        public:
            /* 返回值 value 的具体值取决于最终选择了哪一个 test 函数 */
            static constexpr bool value =
                    std::is_same<decltype(test<T>(nullptr)), char>::value;
        };
        
        struct S {
            S() = delete;
        };
        
        void test() {
            cout << IsDefaultConstructibleT<int>::value << endl;
            
            cout << IsDefaultConstructibleT<S>::value << endl;
            
        }
    
    }
    
    /*
远在 1998 年发布第一版 C++标准之前，基于 SFINAE 的萃取的实现就已经成为了可能。
     该方法的核心一致都是实现两个返回值类型不同的重载函数模板：
        template<...> static char test(void*);
        template<...> static long test(...);
        
    但是，在最早的实现技术中，会基于返回值类型的大小来判断使用了哪一个重载函数（
         也会用到 0 和 enum，因为在当时 nullptr 和 constexpr 还没有被引入）：
            enum { value = sizeof(test<...>(0)) == 1 };
     */
    namespace case2 {
        
        template<typename T>
        struct IsDefaultConstructibleHelper {
        private:
            // test() trying substitute call of a default constructor for T passed as U:
            template<typename U, typename = decltype(U())>
            static std::true_type test(void*);
            
            // test() fallback:
            template<typename>
            static std::false_type test(...);
        public:
            using Type = decltype(test<T>(nullptr));
        };
        
        template<typename T>
        struct IsDefaultConstructibleT : IsDefaultConstructibleHelper<T>::Type { };
        
        void test() {
            cout << IsDefaultConstructibleT<int>::value << endl;
        }
    }
    
    /*
19.4.2 用 SFINAE 排除偏特化
    另一种实现基于 SFINAE 的萃取的方式会用到偏特化。这里，我们同样可以使用上文中用来
    判断类型 T 是否是可以被默认初始化的例子：
    #include "issame.hpp"
    #include <type_traits> //defines true_type and false_type
    // 别名模板，helper to ignore any number of template parameters:
    template<typename ...> using VoidT = void;
    // primary template:
    template<typename, typename = VoidT<>>
    struct IsDefaultConstructibleT : std::false_type
    { };
    // partial specialization (may be SFINAE’d away):
    template<typename T>
    struct IsDefaultConstructibleT<T, VoidT<decltype(T())>> :
    std::true_type
    { }
    ;
    和上文中优化之后的 IsDefaultConstructibleT 预测萃取类似，我们让适用于一般情况的版本继
    承自 std::false_type，因为默认情况下一个类型没有 size_type 成员。
    此处一个比较有意思的地方是，第二个模板参数的默认值被设定为一个辅助别名模板 VoidT。
    这使得我们能够定义各种使用了任意数量的编译期类型构造的偏特化。
    针对我们的例子，只需要一个类型构造：
    decltype(T())
    这样就可以检测类型 T 是否是可以被默认初始化的。如果对于某个特定的类型 T，其默认构
    造函数是无效的，此时 SIFINEAE 就是使该偏特化被丢弃掉，并最终使用主模板。否则该偏
    特化就是有效的，并且会被选用。
    在 C++17 中，C++标准库引入了与 VoidT 对应的类型萃取 std::void_t<>。在 C++17 之前，向上
    面那样定义我们自己的 std::void_t 是很有用的，甚至可以将其定义在 std 命名空间里：
    #include <type_traits>
    #ifndef __cpp_lib_void_t
    namespace std {
    template<typename...> using void_t = void;
    }
    #endif
    从 C++14 开始，C++标准委员会建议通过定义预先达成一致的特征宏（feature macros）来标
    识那些标准库的内容以及被实现了。这并不是标准的强制性要求，但是实现者通常都会遵守
    这一建议，以为其用户提供方便。__cpp_lib_void_t 就是被建议用来标识在一个库中是否实
    现了 std::void_t 的宏，所以在上面的 code 中我们将其用于了条件判断。
    很显然，这一定义类型萃取的方法看上去要比之前介绍的使用了函数模板重载的方法精简的
    多。但是该方法要求要能够将相应的条件放进模板参数的声明中。而使用了函数重载的类模
    板则使得我们能够使用额外的辅助函数或者辅助类。
     */
    
    namespace case3 {
        // 别名模板，helper to ignore any number of template parameters:
        template<typename ...> using VoidT = void;
        
        // primary template:
        template<typename, typename = VoidT<>>
        struct IsDefaultConstructibleT : std::false_type { };
        
        // partial specialization (may be SFINAE’d away):
        template<typename T>
        struct IsDefaultConstructibleT<T, VoidT<decltype(T())>> : std::true_type { };
    
    }
    
    /*
19.4.3 将泛型 Lambdas 用于 SFINAE（Using Generic Lambdas for SFINAE）
    无论使用哪一种技术，在定义萃取的时候总是需要用到一些样板代码：重载并调用两个 test()
    成员函数，或者实现多个偏特化。接下来我们会展示在 C++17 中，如何通过指定一个泛型
    lambda 来做条件测试，将样板代码的数量最小化。
    作为开始，先介绍一个用两个嵌套泛型 lambda 表达式构造的工具：
    #include <utility>
    // helper: checking validity of f (args...) for F f and Args... args:
    template<typename F, typename... Args,
    typename = decltype(std::declval<F>() (std::declval<Args&&>()...))>
    std::true_type isValidImpl(void*);
    // fallback if helper SFINAE’d out:
    template<typename F, typename... Args>
    std::false_type isValidImpl(...);
    // define a lambda that takes a lambda f and returns whether calling
    f with args is valid
    inline constexpr
    auto isValid = [](auto f) {
    return [](auto&&... args) {
    return decltype(isValidImpl<decltype(f),
    decltype(args)&&...>(nullptr)){};
    };
    };
    // helper template to represent a type as a value
    template<typename T>
    struct TypeT {
    using Type = T;
    };
    // helper to wrap a type as a value
    template<typename T>
    constexpr auto type = TypeT<T>{};
    // helper to unwrap a wrapped type in unevaluated contexts
    五车书馆
    208
    template<typename T>
    T valueT(TypeT<T>); // no definition needed
    先从 isValid 的定义开始：它是一个类型为 lambda 闭包的 constexpr 变量。声明中必须要使
    用一个占位类型（placeholder type，代码中的 auto），因为 C++没有办法直接表达一个闭包
    类型。在 C++17 之前，lambda 表达式不能出现在 const 表达式中，因此上述代码只有在 C++17
    中才有效。因为 isValid 是闭包类型的，因此它可以被调用，但是它被调用之后返回的依然
    是一个闭包类型，返回结果由内部的 lambda 表达式生成。
    在深入讨论内部的 lambda 表达式之前，先来看一个 isValid 的典型用法：
    constexpr auto isDefaultConstructible
    = isValid([](auto x) -> decltype((void)decltype(valueT(x))() {});
    我们已经知道 isDefaultConstructible 的类型是闭包类型，而且正如其名字所暗示的
    那样，它是一个可以被用来测试某个类型是不是可以被默认构造的函数对象。也就是说，
    isValid 是一个萃取工厂（traits factory）：它会为其参数生成萃取，并用生成的萃取对对象进
    行测试。
    辅助变量模板 type 允许我们用一个值代表一个类型。对于通过这种方式获得的数值 x，我们
    可以通过使用 decltype(valueT(x))得到其原始类型，这也正是上面被传递给 isValid 的 lambda
    所做的事情。如果提取的类型不可以被默认构造，我们要么会得到一个编译错误，要么相关
    联的声明就会被 SFINAE 掉（得益于 isValid 的具体定义，我们代码中所对应的情况是后者）。
    可以像下面这样使用 isDefaultConstructible：
    isDefaultConstructible(type<int>) //true (int is
    defaultconstructible)
    isDefaultConstructible(type<int&>)
    //false
    (references
    are
    not
    default-constructible)
    为 了 理 解 各 个 部 分 是 如 何 工 作 的 ， 先 来 看 看 当 isValid 的 参 数 f 被 绑 定 到
    isDefaultConstructible 的泛型 lambda 参数时，isValid 内部的 lambda 表达式会变成什
    么样子。通过对 isValid 的定义进行替换，我们得到如下等价代码：
    constexpr auto isDefaultConstructible= [](auto&&... args) {
    return decltype(isValidImpl<decltype([](auto x) ->
    decltype((void)decltype(valueT(x))())),
    decltype(args)&&...> (nullptr)){};
    };
    如果我们回头看看第一个 isValidImpl()的定义，会发现它还有一个如下形式的默认模板参数：
    decltype(std::declval<F>()(std::declval<Args&&>()...))>
    它试图对第一个模板参数的值进行调用，而这第一个参数正是 isDefaultConstructible
    定 义 中 的 lambda 的 闭 包 类 型 ， 调 用 参 数 为 传 递 给 isDefaultConstructible 的
    (decltype(args)&&...)类型的值。由于 lambda 中只有一个参数 x，因此 args 就需要扩展成一个
    五车书馆
    209
    参数；在我们上面的 static_assert 例子中，参数类型伟 TypeT<int>或者 TypeT<int&>。对于
    TypeT<int&>的情况，decltype(valueT(x))的结果是 int&，此时 decltype(valueT(x))()是无效的，
    因此在第一个 isValidImpl()的声明中默认模板参数的替换会失败，从而该 isValidImpl()声明会
    被 SFINAE 掉。这样就只有第二个声明可用，且其返回值类型为 std::false_type。
    整体而言，在传递 type<int&>的时候，isDefaultConstructible 会返回 false_type。而
    如果传递的是 type<int>的话，替换不会失败，因此第一个 isValidImpl()的声明会被选择，返
    回结果也就是 true_type 类型的值。
    会议一下，为了使 SFINAE 能够工作，替换必须发生在被替换模板的立即上下文（immediate
    context，参见第 15.7.1 节）中。在我们这个例子中，被替换的模板是 isValidImpl 的第一个声
    明，而且泛型 lambda 的调用运算符被传递给了 isValid。因此，被测试的内容必须出现在
    lambda 的返回类型中，而不是函数体中。
    我们的 isDefaultConstructible 萃取和之前的萃取在实现上有一些不同，主要体现在
    它需要执行函数形式的调用，而不是指定模板参数。这可能是一种更为刻度的方式，但是也
    可以按照之前的方式实现：
    template<typename T>using IsDefaultConstructibleT
    = decltype(isDefaultConstructible(std::declval<T>()));
    虽 然 这 是 传 统 的 模 板 声 明 方 式 ， 但 是 它 只 能 出 现 在 namespace 作 用 域 内 ， 然 而
    isDefaultConstructible 的定义却很可能被在一个块作用域内引入。
    到目前为止，这一技术看上去好像并没有那么有竞争力，因为无论是实现中涉及的表达式还
    是其使用方式都要比之前的技术复杂得多。但是，一旦有了 isValid，并且对其进行了很好的
    理解，有很多萃取都可以只用一个声明实现。比如，对是否能够访问名为 first 的成员进行
    测试，就非常简洁（完整的例子请参见 19.6.4 节）：
    constexpr auto hasFirst
    = isValid([](auto x) -> decltype((void)valueT(x).first) {});
     */
    namespace case4 {
        
        // helper: checking validity of f (args...) for F f and Args... args:
        template<typename F, typename... Args,
        typename = decltype(std::declval<F>() (std::declval<Args&&>()...))>
        std::true_type isValidImpl(void*);
        
        // fallback if helper SFINAE’d out:
        template<typename F, typename... Args>
        std::false_type isValidImpl(...);
        
        // define a lambda that takes a lambda f and returns whether calling f with args is valid
        inline constexpr auto isValid = [](auto f) {
            
            return [](auto&&... args) {
                return decltype(isValidImpl<decltype(f),
                        decltype(args)&&...>(nullptr)){};
            };
        };
        
        // helper template to represent a type as a value
        template<typename T>
        struct TypeT {
            using Type = T;
        };
        
        // helper to wrap a type as a value
        template<typename T>
        constexpr auto type = TypeT<T>{};
        
        // helper to unwrap a wrapped type in unevaluated contexts
        template<typename T>
        T valueT(TypeT<T>); // no definition needed
    
    
        // 在深入讨论内部的 lambda 表达式之前，先来看一个 isValid 的典型用法：
        /*
    我们已经知道 isDefaultConstructible 的类型是闭包类型，而且正如其名字所暗示的那样，
         它是一个可以被用来测试某个类型是不是可以被默认构造的函数对象。
         
         也就是说，isValid 是一个萃取工厂（traits factory）：
             它会为其参数生成萃取，并用生成的萃取对对象进行测试。
         */
        constexpr auto isDefaultConstructible = isValid([](auto x) -> decltype((void)decltype(valueT(x))() ){});
    
        void test() {
            // 可以像下面这样使用 isDefaultConstructible：
            cout << isDefaultConstructible(type<int>) << endl;          // true (int is defaultconstructible)
            cout << isDefaultConstructible(type<int&>) << endl;         // false (references are not default-constructible)
        }
    
        
        /*
        // ERROR
        constexpr auto isDefaultConstructible2 = [](auto&&... args) {
        return decltype(isValidImpl<
                decltype([](auto x) -> decltype((void)decltype(valueT(x))), decltype(args)&&...)> (nullptr)
                ){};
        };
        */
        /*
        decltype(
            isValidImpl<
            
                            decltype(
                                       [](auto x) -> decltype(
                                                                (void)decltype(valueT(x))
                                                             ),      decltype(args) && ...
                                     )
                        
                       > (nullptr)
                )
        */
        
    }
    
    namespace case5 {
    
    }

    class Tmp {
    public:
    
    };

}

int
main()
//main_SFINAE_Based_Traits_19_4()
{
    ch19_4::case4::test();

    return 0;
}