//
// Created by yangfeng on 2022/8/8.
//

#include "../headers.h"

/*
19.8 类型分类（Type Classification）
    在某些情况下，如果能够知道一个模板参数的类型是内置类型，指针类型，class 类型，或
    者是其它什么类型，将会很有帮助。在接下来的章节中，我们定义了一组类型萃取，通过它
    们我们可以判断给定类型的各种特性。这样我们就可以单独为特定的某些类型编写代码：
    if (IsClassT<T>::value) {
    ...
    }
    或者是将其用于编译期 if（在 C++17 中引入）以及某些为了萃取的便利性而引入的特性（参
    见第 19.7.3 节）：
    if constexpr (IsClass<T>) {
    ...
    }
    或者时将其用于偏特化：
    template<typename T, bool = IsClass<T>>
    class C { //primary template for the general case
    ...
    };
    template<typename T>
    class C<T, true> { //partial specialization for class types
    ...
    };
    此外，诸如 IsPointerT<T>::value 一类的表达式的结果是 bool 型常量，因此它们也将是有效
    的非类型模板参数。这样，就可以构造更为高端和强大的模板，这些模板可以被基于它们的
    五车书馆
    232
    类型参数的特性进行特化。
    C++标准库定义了一些类似的萃取，这些萃取可以判断一个类型的主要种类或者是该类型被
    复合之后的种类。更多细节请参见第 D.2.2 节和第 D.2.1 节。
    19.8.1 判断基础类型（Determining Fundamental Types）
    作为开始，我们先定义一个可以判断某个类型是不是基础类型的模板。默认情况下，我们认
    为类型不是基础类型，而对于基础类型，我们分别进行了特化：
    #include <cstddef> // for nullptr_t
    #include <type_traits> // for true_type, false_type, and
    bool_constant<>
    // primary template: in general T is not a fundamental type
    template<typename T>
    struct IsFundaT : std::false_type {
    };
    // macro to specialize for fundamental types
    #define MK_FUNDA_TYPE(T) \
    template<> struct IsFundaT<T> : std::true_type { \
    };
    MK_FUNDA_TYPE(void)
    MK_FUNDA_TYPE(bool)
    MK_FUNDA_TYPE(char)
    MK_FUNDA_TYPE(signed char)
    MK_FUNDA_TYPE(unsigned char)
    MK_FUNDA_TYPE(wchar_t)
    MK_FUNDA_TYPE(char16_t)
    MK_FUNDA_TYPE(char32_t)
    MK_FUNDA_TYPE(signed short)
    MK_FUNDA_TYPE(unsigned short)
    MK_FUNDA_TYPE(signed int)
    MK_FUNDA_TYPE(unsigned int)
    MK_FUNDA_TYPE(signed long)
    MK_FUNDA_TYPE(unsigned long)
    MK_FUNDA_TYPE(signed long long)
    MK_FUNDA_TYPE(unsigned long long)
    MK_FUNDA_TYPE(float)
    MK_FUNDA_TYPE(double)
    MK_FUNDA_TYPE(long double)
    MK_FUNDA_TYPE(std::nullptr_t)
    #undef MK_FUNDA_TYPE
    主模板定义了常规情况。也就是说，通常而言 IfFundaT<T>::value 会返回 false：
    五车书馆
    233
    template<typename T>
    struct IsFundaT : std::false_type {
    static constexpr bool value = false;
    };
    对于每一种基础类型，我们都进行了特化，因此 IsFundaT<T>::value 的结果也都会返回 true。
    为了简单，我们定义了一个可以扩展成所需代码的宏。比如：
    MK_FUNDA_TYPE(bool)
    会扩展成：
    template<> struct IsFundaT<bool> : std::true_type {
    static constexpr bool value = true;
    };
    下面的例子展示了该模板的一种可能的应用场景：
    #include "isfunda.hpp"
    #include <iostream>
    template<typename T>
    void test (T const&)
    {
    if (IsFundaT<T>::value) {
    std::cout << "T is a fundamental type" << ’\n’;}
    else {
    std::cout << "T is not a fundamental type" << ’\n’;
    }
    }
    int main()
    {
    test(7);
    test("hello");
    }
    其输出如下：
    T is a fundamental type
    T is not a fundamental type
    采用同样会的方式，我们也可以定义类型函数 IsIntegralT 和 IsFloatingT 来区分哪些类型是整
    形标量类型以及浮点型标量类型。
    C++标准库采用了一种更为细粒度的方法来测试一个类型是不是基础类型。它先定义了主要
    的类型种类，每一种类型都被匹配到一个相应的种类（参见第 D.2.1 节），然后合成诸如
    std::is_integral 和 std::is_fundamental 类型种类（参见第 D2.2.节）。
    五车书馆
    234
    19.8.2 判断复合类型
    复合类型是由其它类型构建出来的类型。简单的复合类型包含指针类型，左值以及右值引用
    类型，指向成员的指针类型（pointer-to-member types），和数组类型。它们是由一种或者
    两种底层类型构造的。Class 类型以及函数类型同样也是复合类型，但是它们可能是由任意
    数量的类型组成的。在这一分类方法中，枚举类型同样被认为是复杂的符合类型，虽然它们
    不是由多种底层类型构成的。简单的复合类型可以通过偏特化来区分。
    指针
    我们从指针类型这一简单的分类开始：
    template<typename T>
    struct IsPointerT : std::false_type { //primary template: by default
    not a pointer
    };
    template<typename T>
    struct IsPointerT<T*> : std::true_type { //partial specialization for
    pointers
    using BaseT = T; // type pointing to
    };
    主模板会捕获所有的非指针类型，和往常一样，其值为 fase 的 value 成员是通过基类
    std::false_type 提供的，表明该类型不是指针。偏特化实现会捕获所有的指针类型（T*），
    其为 true 的成员 value 表明该类型是一个指针。偏特化实现还额外提供了类型成员 BaseT，
    描述了指针所指向的类型。注意该类型成员只有在原始类型是指针的时候才有，从其使其变
    成 SFINAE 友好的类型萃取。
    C++标准库也提供了相对应的萃取 std::is_pointer<>，但是没有提供一个成员类型来描述指针
    所指向的类型。相关描述详见第 D.2.1 节。
    引用
    相同的方法也可以被用来识别左值引用：
    template<typename T>
    struct IsLValueReferenceT : std::false_type { //by default no lvalue
    reference
    };
    template<typename T>
    五车书馆
    235
    struct IsLValueReferenceT<T&> : std::true_type { //unless T is lvalue
    references
    using BaseT = T; // type referring to
    };
    以及右值引用：
    template<typename T>
    struct IsRValueReferenceT : std::false_type { //by default no rvalue
    reference
    };
    template<typename T>
    struct IsRValueReferenceT<T&&> : std::true_type { //unless T is rvalue
    reference
    using BaseT = T; // type referring to
    };
    它俩又可以被组合成 IsReferenceT<>萃取：
    #include "islvaluereference.hpp"
    #include "isrvaluereference.hpp"
    #include "ifthenelse.hpp"
    template<typename T>
    class IsReferenceT
    : public IfThenElseT<IsLValueReferenceT<T>::value,
    IsLValueReferenceT<T>,
    IsRValueReferenceT<T>
    >::Type {
    };
    在这一实现中，我们用 IfThenElseT 从 ISLvalueReference<T>和 IsRValueReferenceT<T>中选择基
    类，这里还用到了元函数转发（参见第 19.3.2 节）。如果 T 是左值引用，我们会从
    IsLReference<T>做继承，并通过继承得到相应的 value 和 BaseT 成员。否则，我们就从
    IsRValueReference<T>做继承，它会判断一个类型是不是右值引用（并未相应的情况提供对应
    的成员）。
    C++标准库也提供了相应的 std::is_lvalue_reference<>和 std::is_rvalue_reference<>萃取（相关
    介绍请参见第 D.2.1 节），还有 std::is_reference<>（相关介绍请参见第 D.2.2 节）。同样的，
    这些萃取也没有提供代表其所引用的类型的类型成员。
    数组
    在定义可以判断数组的萃取时，让人有些意外的是偏特化实现中的模板参数数量要比主模板
    五车书馆
    236
    多：
    #include <cstddef>
    template<typename T>
    struct IsArrayT : std::false_type { //primary template: not an array
    };
    template<typename T, std::size_t N>
    struct IsArrayT<T[N]> : std::true_type { //partial specialization for
    arrays
    using BaseT = T;
    static constexpr std::size_t size = N;
    };
    template<typename T>
    struct IsArrayT<T[]> : std::true_type { //partial specialization for
    unbound arrays
    using BaseT = T;
    static constexpr std::size_t size = 0;
    };
    在这里，多个额外的成员被用来描述被用来分类的数组的信息：数组的基本类型和大小（0
    被用来标识未知大小的数组的尺寸）。
    C++标准库提供了相应的 std::is_array<>来判断一个类型是不是数组，在第 D.2.1 节有其相关
    介绍。除此之外，诸如 std::rank<>和 std::extent<>之类的萃取还允许我们去查询数组的维度
    以及某个维度的大小（参见第 D.3.1 节）。
    指向成员的指针（Pointers to Members）
    也可以用相同的方式处理指向成员的指针：
    template<typename T>
    struct IsPointerToMemberT : std::false_type { //by default no
    pointer-to-member
    };
    template<typename T, typename C>
    struct IsPointerToMemberT<T C::*> : std::true_type { //partial
    specialization
    using MemberT = T;
    using ClassT = C;
    };
    五车书馆
    237
    这里额外的成员（MemberT 和 ClassT）提供了与成员的类型以及 class 的类型相关的信息。
    C++ 标 准 库 提 供 了 更 为 具 体 的 萃 取 ， std::is_member_object_pointer<> 和
    std::is_member_function_pointer<> ， 详 见 第 D.2.1 节 ， 还 有 在 第 D.2.2 节 介 绍 的
    std::is_member_pointer<>。
    19.8.3 识别函数类型（Identifying Function Types）
    函数类型比较有意思，因为它们除了返回类型，还可能会有任意数量的参数。因此，在匹配
    一个函数类型的偏特化实现中，我们用一个参数包来捕获所有的参数类型，就如同我们在
    19.3.2 节中对 DecayT 所做的那样：
    #include "../typelist/typelist.hpp"
    template<typename T>
    struct IsFunctionT : std::false_type { //primary template: no function
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params...)> : std::true_type
    { //functions
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = false;
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params..., ...)> : std::true_type { //variadic
    functions
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = true;
    };
    上述实现中函数类型的每一部分都被暴露了出来：返回类型被 Type 标识，所有的参数都被
    作为 ParamsT 捕获进了一个 typelist 中（在第 24 章有关于 typelist 的介绍），而可变参数（...）
    表示的是当前函数类型使用的是不是 C 风格的可变参数。
    不幸的是，这一形式的 IsFunctionT 并不能处理所有的函数类型，因为函数类型还可以包含
    const 和 volatile 修饰符，以及左值或者右值引用修饰符（参见第 C.2.1 节），在 C++17 之后，
    还有 noexcept 修饰符。比如：
    using MyFuncType = void (int&) const;
    这一类函数类型只有在被用于非 static 成员函数的时候才有意义，但是不管怎样都算得上是
    函数类型。而且，被标记为 const 的函数类型并不是真正意义上的 const 类型，因此
    五车书馆
    238
    RemoveConst 并不能将 const 从函数类型中移除。因此，为了识别有限制符的函数类型，我
    们需要引入一大批额外的偏特化实现，来覆盖所有可能的限制符组合（每一个实现都需要包
    含 C 风格和非 C 风格的可变参数情况）。这里，我们只展示所有偏特化实现中的 5 中情况：
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params...) const> : std::true_type {
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = false;
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params..., ...) volatile> : std::true_type {
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = true;
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params..., ...) const volatile> : std::true_type {
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = true;
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params..., ...) &> : std::true_type {
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = true;
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params..., ...) const&> : std::true_type {
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = true;
    };
    ...
    当所有这些都准备完毕之后，我们就可以识别除 class 类型和枚举类型之外的所有类型了。
    我们会在接下来的章节中除了这两种例外情况。
    C++标准库也提供了相应的 std::is_function<>萃取，详细介绍请参见第 D.2.1 节。
    五车书馆
    239
    19.8.4 判断 class 类型（Determining Class Types）
    和到目前为止我们已经处理的各种复合类型不同，我们没有相应的偏特化模式来专门匹配
    class 类型。也不能像处理基础类型一样一一列举所有的 class 类型。相反，我们需要用一种
    间接的方法来识别 class 类型，为此我们需要找出一些适用于所有 class 类型的类型或者表达
    式（但是不能适用于其它类型）。有着这样的类型或者表达式之后，我们就可以使用在第
    19.4 节介绍的 SFINAE 萃取技术了。
    Class 中可以被我们用来识别 class 类型的最为方便的特性是：只有 class 类型可以被用于指
    向成员的指针类型（pointer-to-member types）的基础。也就是说，对于 X Y::*一类的类型结
    构，Y 只能是 class 类型。下面的 IsClassT<>就利用了这一特性（将 X 随机选择为 int）：
    #include <type_traits>
    template<typename T, typename = std::void_t<>>
    struct IsClassT : std::false_type { //primary template: by default no
    class
    };
    template<typename T>
    struct IsClassT<T, std::void_t<int T::*>> // classes can have
    pointer-to-member
    : std::true_type {
    };
    C++语言规则指出，lambda 表达式的类型是“唯一的，未命名的，非枚举 class 类型”。因
    此在将 IsClassT 萃取用于 lambda 表达时，我们得到的结果是 true：
    auto l = []{};
    static_assert<IsClassT<decltype(l)>::value, "">; //succeeds
    需要注意的是，int T::*表达式同样适用于 unit 类型（更具 C++标准，枚举类型也是 class 类
    型）。
    C++标准库提供了 std::is_class<>和 std::is_union 萃取，在第 D.2.1 节有关于它们的介绍。但是，
    这些萃取需要编译期进行专门的支持，因为目前还不能通过任何核心的语言技术（standard
    core language techniques）将 class 和 struct 从 union 类型中分辨出来。
    19.8.5 识别枚举类型（Determining Enumeration Types）
    目前通过我们已有的萃取技术还唯一不能识别的类型是枚举类型。我们可以通过编写基于
    SFINAE 的萃取来实现这一功能，这里首先需要测试是否可以像整形类型（比如 int）进行显
    式转换，然后依次排除基础类型，class 类型，引用类型，指针类型，还有指向成员的指针
    类型（这些类型都可以被转换成整形类型，但是都不是枚举类型）。但是也有更简单的方法，
    因为我们发现所有不属于其它任何一种类型的类型就是枚举类型，这样就可以像下面这样实
    五车书馆
    240
    现该萃取：
    template<typename T>
    struct IsEnumT {
    static constexpr bool value = !IsFundaT<T>::value
    && !IsPointerT<T>::value &&
    !IsReferenceT<T>::value
    && !IsArrayT<T>::value &&
    !IsPointerToMemberT<T>::value
    && !IsFunctionT<T>::value &&
    !IsClassT<T>::value;
    };
    C++标准库提供了相对应的 std::is_enum<>萃取，在第 D.2.1 节有对其进行介绍。通常，为了
    提高编译性能，编译期会直接提供这一类萃取，而不是将其实现为其它的样子。
 */

using namespace std;

namespace ch19_8 {
    /*
    在某些情况下，如果能够知道一个模板参数的类型是内置类型，指针类型，class 类型，或
者是其它什么类型，将会很有帮助。在接下来的章节中，我们定义了一组类型萃取，通过它
们我们可以判断给定类型的各种特性。这样我们就可以单独为特定的某些类型编写代码
     */

    namespace case1 {
        // primary template: in general T is not a fundamental type
        template<typename T>
        struct IsFundaT : std::false_type {
            static constexpr bool value = false;
        };
        
        // macro to specialize for fundamental types
        #define MK_FUNDA_TYPE(T) \
        template<>               \
        struct IsFundaT<T> : std::true_type { };
                MK_FUNDA_TYPE(void)
                MK_FUNDA_TYPE(bool)
                MK_FUNDA_TYPE(char)
                MK_FUNDA_TYPE(signed char)
                MK_FUNDA_TYPE(unsigned char)
                MK_FUNDA_TYPE(wchar_t)
                MK_FUNDA_TYPE(char16_t)
                MK_FUNDA_TYPE(char32_t)
                MK_FUNDA_TYPE(signed short)
                MK_FUNDA_TYPE(unsigned short)
                MK_FUNDA_TYPE(signed int)
                MK_FUNDA_TYPE(unsigned int)
                MK_FUNDA_TYPE(signed long)
                MK_FUNDA_TYPE(unsigned long)
                MK_FUNDA_TYPE(signed long long)
                MK_FUNDA_TYPE(unsigned long long)
                MK_FUNDA_TYPE(float)
                MK_FUNDA_TYPE(double)
                MK_FUNDA_TYPE(long double)
                MK_FUNDA_TYPE(std::nullptr_t)
        #undef MK_FUNDA_TYPE

        template<typename T>
        void test_helper (T const&) {
            
            if (IsFundaT<T>::value)
                std::cout << "T is a fundamental type" << endl;
            else
                std::cout << "T is not a fundamental type" << endl;
            
        }
        
        void test() {
            test_helper(7);
            test_helper("hello");
        }
    }
    
    /*
19.8.2 判断复合类型
    复合类型是由其它类型构建出来的类型。简单的复合类型包含指针类型，左值以及右值引用
    类型，指向成员的指针类型（pointer-to-member types），和数组类型。它们是由一种或者
    两种底层类型构造的。Class 类型以及函数类型同样也是复合类型，但是它们可能是由任意
    数量的类型组成的。在这一分类方法中，枚举类型同样被认为是复杂的符合类型，虽然它们
    不是由多种底层类型构成的。简单的复合类型可以通过偏特化来区分。
     */
    
    namespace case2 {
        // 指针
        template<typename T>
        struct IsPointerT : std::false_type { };        //primary template: by default not a pointer
        
        template<typename T>
        struct IsPointerT<T*> : std::true_type {        //partial specialization for pointers
            using BaseT = T;                            // type pointing to
        };
    
        // 相同的方法也可以被用来识别左值引用：
        template<typename T>
        struct IsLValueReferenceT
                : std::false_type { };                  //by default no lvalue reference
        
        
        template<typename T>
        struct IsLValueReferenceT<T&> : std::true_type {//unless T is lvalue references
            using BaseT = T;                            // type referring to
        };
        
        // 以及右值引用：
        template<typename T>
        struct IsRValueReferenceT
                : std::false_type { };                  //by default no rvalue reference
        
        template<typename T>
        struct IsRValueReferenceT<T&&> : std::true_type {//unless T is rvalue reference
            using BaseT = T; // type referring to
        };
        
        // 数组
        //    在定义可以判断数组的萃取时，让人有些意外的是偏特化实现中的模板参数数量要比主模板多：
        template<typename T>
        struct IsArrayT : std::false_type { };          //primary template: not an array
        
        template<typename T, std::size_t N>
        struct IsArrayT<T[N]> : std::true_type {        //partial specialization for arrays
            using BaseT = T;
            static constexpr std::size_t size = N;
        };
        
        template<typename T>
        struct IsArrayT<T[]> : std::true_type {         //partial specialization for unbound arrays
            using BaseT = T;
            static constexpr std::size_t size = 0;
        };
        
        /*
        C++标准库提供了相应的 std::is_array<>来判断一个类型是不是数组，在第 D.2.1 节有其相关
介绍。除此之外，诸如 std::rank<>和 std::extent<>之类的萃取还允许我们去查询数组的维度
以及某个维度的大小（参见第 D.3.1 节）。
         */
        
    }
    
    namespace case3 {
        /*
    指向成员的指针（Pointers to Members）
        也可以用相同的方式处理指向成员的指针：
        template<typename T>
        struct IsPointerToMemberT : std::false_type { //by default no
        pointer-to-member
        };
        template<typename T, typename C>
        struct IsPointerToMemberT<T C::*> : std::true_type { //partial
        specialization
        using MemberT = T;
        using ClassT = C;
        };
        五车书馆
        237
        这里额外的成员（MemberT 和 ClassT）提供了与成员的类型以及 class 的类型相关的信息。
        C++ 标 准 库 提 供 了 更 为 具 体 的 萃 取 ， std::is_member_object_pointer<> 和
        std::is_member_function_pointer<> ， 详 见 第 D.2.1 节 ， 还 有 在 第 D.2.2 节 介 绍 的
        std::is_member_pointer<>。
         */

        template<typename T>
        struct IsPointerToMemberT : std::false_type {           //by default no pointer-to-member
        };
        
        template<typename T, typename C>
        struct IsPointerToMemberT<T C::*> : std::true_type {    //partial specialization
            using MemberT = T;
            using ClassT = C;
        };
        
        class Test {
        public:
            int a;
            void func() { }
        };
        
        void test() {
            Test t { .a = 0 };
            cout << IsPointerToMemberT<decltype(&Test::func)>::value << endl;
            cout << IsPointerToMemberT<decltype(&Test::a)>::value << endl;
            
            
        }
        
    }
    
    /*
19.8.3 识别函数类型（Identifying Function Types）
    函数类型比较有意思，因为它们除了返回类型，还可能会有任意数量的参数。因此，在匹配
    一个函数类型的偏特化实现中，我们用一个参数包来捕获所有的参数类型，就如同我们在
    19.3.2 节中对 DecayT 所做的那样：
    #include "../typelist/typelist.hpp"
    template<typename T>
    struct IsFunctionT : std::false_type { //primary template: no function
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params...)> : std::true_type
    { //functions
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = false;
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params..., ...)> : std::true_type { //variadic
    functions
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = true;
    };
    上述实现中函数类型的每一部分都被暴露了出来：返回类型被 Type 标识，所有的参数都被
    作为 ParamsT 捕获进了一个 typelist 中（在第 24 章有关于 typelist 的介绍），而可变参数（...）
    表示的是当前函数类型使用的是不是 C 风格的可变参数。
    不幸的是，这一形式的 IsFunctionT 并不能处理所有的函数类型，因为函数类型还可以包含
    const 和 volatile 修饰符，以及左值或者右值引用修饰符（参见第 C.2.1 节），在 C++17 之后，
    还有 noexcept 修饰符。比如：
    using MyFuncType = void (int&) const;
    这一类函数类型只有在被用于非 static 成员函数的时候才有意义，但是不管怎样都算得上是
    函数类型。而且，被标记为 const 的函数类型并不是真正意义上的 const 类型，因此
    五车书馆
    238
    RemoveConst 并不能将 const 从函数类型中移除。因此，为了识别有限制符的函数类型，我
    们需要引入一大批额外的偏特化实现，来覆盖所有可能的限制符组合（每一个实现都需要包
    含 C 风格和非 C 风格的可变参数情况）。这里，我们只展示所有偏特化实现中的 5 中情况：
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params...) const> : std::true_type {
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = false;
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params..., ...) volatile> : std::true_type {
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = true;
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params..., ...) const volatile> : std::true_type {
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = true;
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params..., ...) &> : std::true_type {
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = true;
    };
    template<typename R, typename... Params>
    struct IsFunctionT<R (Params..., ...) const&> : std::true_type {
    using Type = R;
    using ParamsT = Typelist<Params...>;
    static constexpr bool variadic = true;
    };
    ...
    当所有这些都准备完毕之后，我们就可以识别除 class 类型和枚举类型之外的所有类型了。
    我们会在接下来的章节中除了这两种例外情况。
    C++标准库也提供了相应的 std::is_function<>萃取，详细介绍请参见第 D.2.1 节。
     */
    
    namespace case4 {
        // 自己定义的类，为了消除错误提示
        template <typename ... Types>
        struct Typelist { };
        
        template<typename T>
        struct IsFunctionT : std::false_type { };  //primary template: no function
        
        
        template<typename R, typename... Params>
        struct IsFunctionT<R (Params...)> : std::true_type { //functions
            using Type = R;
            using ParamsT = Typelist<Params...>;
            static constexpr bool variadic = false;
        };
            
        template<typename R, typename... Params>
        struct IsFunctionT<R (Params..., ...)> : std::true_type { //variadic functions
            using Type = R;
            using ParamsT = Typelist<Params...>;
            static constexpr bool variadic = true;
        };
        
        /*
    上述实现中函数类型的每一部分都被暴露了出来：返回类型被 Type 标识，所有的参数都被
        作为 ParamsT 捕获进了一个 typelist 中（在第 24 章有关于 typelist 的介绍），而可变参数（...）
        表示的是当前函数类型使用的是不是 C 风格的可变参数。
        不幸的是，这一形式的 IsFunctionT 并不能处理所有的函数类型，因为函数类型还可以包含
        const 和 volatile 修饰符，以及左值或者右值引用修饰符（参见第 C.2.1 节），在 C++17 之后，
        还有 noexcept 修饰符。比如：
        using MyFuncType = void (int&) const;
        这一类函数类型只有在被用于非 static 成员函数的时候才有意义，但是不管怎样都算得上是
        函数类型。而且，被标记为 const 的函数类型并不是真正意义上的 const 类型，因此
        五车书馆
        238
        RemoveConst 并不能将 const 从函数类型中移除。因此，为了识别有限制符的函数类型，我
        们需要引入一大批额外的偏特化实现，来覆盖所有可能的限制符组合（每一个实现都需要包
        含 C 风格和非 C 风格的可变参数情况）。
         */

        template<typename R, typename... Params>
        struct IsFunctionT<R (Params...) const> : std::true_type {
            using Type = R;
            using ParamsT = Typelist<Params...>;
            static constexpr bool variadic = false;
        };
        
        template<typename R, typename... Params>
        struct IsFunctionT<R (Params..., ...) volatile> : std::true_type {
            using Type = R;
            using ParamsT = Typelist<Params...>;
            static constexpr bool variadic = true;
        };
        
        template<typename R, typename... Params>
        struct IsFunctionT<R (Params..., ...) const volatile> : std::true_type {
            using Type = R;
            using ParamsT = Typelist<Params...>;
            static constexpr bool variadic = true;
        };
        
        template<typename R, typename... Params>
        struct IsFunctionT<R (Params..., ...) &> : std::true_type {
            using Type = R;
            using ParamsT = Typelist<Params...>;
            static constexpr bool variadic = true;
        };
        
        
        template<typename R, typename... Params>
        struct IsFunctionT<R (Params..., ...) const&> : std::true_type {
            using Type = R;
            using ParamsT = Typelist<Params...>;
            static constexpr bool variadic = true;
        }; 
        
        
    }
    
    namespace case5 {
    
    }

    class Tmp {
    public:
    
    };

}

int
main()
//main_Type_Classification_19_8()
{
    ch19_8::case3::test();
    

    return 0;
}



