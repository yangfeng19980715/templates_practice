//
// Created by yangfeng on 2022/8/2.
//

#include "../headers.h"
/*
类型函数（Type Function）
最初的示例说明我们可以基于类型定义行为。传统上我们在 C 和 C++里定义的函数可以被更
明确的称为值函数（value functions）：它们接收一些值作为参数并返回一个值作为结果。
对于模板，我们还可以定义类型函数（type functions）：它们接收一些类型作为参数并返回
一个类型或者常量作为结果。
一个很有用的内置类型函数是 sizeof，它返回了一个代表了给定类型大小（单位是 byte）的
常数。类模板依然可以被用作类型函数。此时类型函数的参数是模板参数，其结果被提取为
成员类型或者成员常量。比如，sizeof 运算符可以被作为如下接口提供：
    #include <cstddef>
    #include <iostream>
    
    template<typename T>
    struct TypeSize {
        static std::size_t const value = sizeof(T);
    };
    
    int main()
    {
        std::cout << "TypeSize<int>::value = " << TypeSize<int>::value << endl;
    }
这看上去可能没有那么有用，因为我们已经有了一个内置的 sizeof 运算符，但是请注意此处
的 TypeSize<T>是一个类型，它可以被作为类模板参数传递。或者说，TypeSize 是一个模板，
也可以被作为模板模板参数传递。
在接下来的内容中，我们设计了一些更为通用的类型函数，可以按照上述方式将它们用作萃
取类。
*/

using namespace std;

namespace ch19_3 {
    
    /*
19.3.1 元素类型（Element Type）
    假设我们有很多的容器模板，比如 std::vector<>和 std::list<>，也可以包含内置数组。我们希
    望得到这样一个类型函数，当给的一个容器类型时，它可以返回相应的元素类型。这可以通
    过偏特化实现：
    #include <vector>
    #include <list>
    template<typename T>
    struct ElementT; // primary template
    template<typename T>
    struct ElementT<std::vector<T>> { //partial specialization for
    std::vector
    using Type = T;
    };
    template<typename T>
    struct ElementT<std::list<T>> { //partial specialization for std::list
    using Type = T;
    };
    …
    template<typename T, std::size_t N>
    struct ElementT<T[N]> { //partial specialization for arrays of known
    五车书馆
    189
    bounds
    using Type = T;
    };
    template<typename T>
    struct ElementT<T[]> { //partial specialization for arrays of unknown
    bounds
    using Type = T;
    };
    …
    注意此处我们应该为所有可能的数组类型提供偏特化（详见第 5.4 节）。
    我们可以想下面这样使用这些类型函数：
    #include "elementtype.hpp"
    #include <vector>
    #include <iostream>
    #include <typeinfo>
    template<typename T>
    void printElementType (T const& c)
    {
    std::cout << "Container of " <<
    typeid(typename ElementT<T>::Type).name() << " elements.\n";
    }
    int main()
    {
    std::vector<bool> s;
    printElementType(s);
    int arr[42];
    printElementType(arr);
    }
    偏特化的使用使得我们可以在容器类型不知道具体类型函数存在的情况下去实现类型函数。
    但是在某些情况下，类型函数是和其所适用的类型一起被设计的，此时相关实现就可以被简
    化。比如，如果容器类型定义了 value_type 成员类型（标准库容器都会这么做），我们就可
    以有如下实现：
    template<typename C>
    struct ElementT {
    using Type = typename C::value_type;
    };
    这个实现可以是默认实现，它不会排除那些针对没有定义成员类型 value_type 的容器的偏特
    五车书馆
    190
    化实现。
    虽然如此，我们依然建议为类模板的类型参数提供相应的成员类型定义，这样在泛型代码中
    就可以更容易的访问它们（和标准库容器的处理方式类似）。下面的代码体现了这一思想：
    template<typename T1, typename T2, …>
    class X {
    public:
    using … = T1;
    using … = T2;
    …
    };
    那么类型函数的作用体现在什么地方呢？它允许我们根据容器类型参数化一个模板，但是又
    不需要提供代表了元素类型和其它特性的参数。比如，相比于使用
    template<typename T, typename C>
    T sumOfElements (C const& c);
    这一需要显式指定元素类型的模板（sumOfElements<int> list），我们可以定义这样一个模板：
    template<typename C>
    typename ElementT<C>::Type sumOfElements (C const& c);
    其元素类型是通过类型函数得到的。
    注意观察萃取是如何被实现为已有类型的扩充的；也就是说，我们甚至可以为基本类型和封
    闭库的类型定义类型函数。
    在上述情况下，ElementT 被称为萃取类，因为它被用来访问一个已有容器类型的萃取（通
    常而言，在这样一个类中可以有多个萃取）。因此萃取类的功能并不仅限于描述容器参数的
    特性，而是可以描述任意“主参数”的特性。
    为了方便，我们可以伟类型函数创建一个别名模板。比如，我们可以引入：
    template<typename T>
    using ElementType = typename ElementT<T>::Type;
    这可以让 sumOfEkements 的定义变得更加简单：
    template<typename C>
    ElementType<C> sumOfElements (C const& c);
     */

    namespace case1 {
    
    }
    
    /*
转换萃取（Transformation Traits）
    除了可以被用来访问主参数类型的某些特性，萃取还可以被用来做类型转换，比如为某个类
    型添加或移除引用、const 以及 volatile 限制符。
    
  删除引用
    比如，我们可以实现一个 RemoveReferenceT 萃取，用它将引用类型转换成其底层对象或者
    函数的类型，对于非引用类型则保持不变：
        template<typename T>
        struct RemoveReferenceT {
        using Type = T;
        };
        template<typename T>
        struct RemoveReferenceT<T&> {
        using Type = T;
        };
        template<typename T>
        struct RemoveReferenceT<T&&> {
        using Type = T;
        };
        
    同样地，引入一个别名模板可以简化上述萃取的使用：
    template<typename T>
    using RemoveReference = typename RemoveReference<T>::Type;
    当类型是通过一个有时会产生引用类型的构造器获得的时候，从一个类型中删除引用会很有
    意义，比如对于在第 15.6 节介绍的关于函数参数类型 T&&的特殊推断规则。
    C++标准库提供了一个相应的 std::remove_reference<>萃取，详见附录 D.4。
    
  添加引用
    我们也可以给一个已有类型添加左值或者右值引用：
        template<typename T>
        struct AddLValueReferenceT {
        using Type = T&;
        };
        template<typename T>
        using AddLValueReference = typename AddLValueReferenceT<T>::Type;
        template<typename T>
        struct AddRValueReferenceT {
        using Type = T&&;
        };
        
        template<typename T>
        using AddRValueReference = typename AddRValueReferenceT<T>::Type;
    引用折叠的规则在这一依然适用（参见第 15.6 节）。比如对于 AddLValueReference<int &&>，
    返回的类型是 int&，因为我们不需要对它们进行偏特化实现。
    如果我们只实现 AddLValueReferenceT 和 AddRValueReferenceT，而又不对它们进行偏特化的
    话，最方便的别名模板可以被简化成下面这样：
    template<typename T>
    using AddLValueReferenceT = T&;
    template<typename T>
    using AddRValueReferenceT = T&&;
    此时不通过类模板的实例化就可以对其进行实例化（因此称得上是一个轻量级过程）。但是
    这样做是由风险的，因此我们依然希望能够针对特殊的情况对这些模板进行特例化。比如，
    如果适用上述简化实现，那么我们就不能将其用于 void 类型。一些显式的特化实现可以被
    用来处理这些情况：
    template<>
    struct AddLValueReferenceT<void> {
    using Type = void;
    };
    template<>
    struct AddLValueReferenceT<void const> {
    using Type = void const;
    };
    template<>
    struct AddLValueReferenceT<void volatile> {
    using Type = void volatile;
    };
    template<>
    struct AddLValueReferenceT<void const volatile> {
    using Type = void const volatile;
    };
    AddRValueReferenceT 的情况与之类似。
    有了这些偏特化之后，上文中的别名模板必须被实现为类模板的形式（不能适用最简单的那
    种形式），这样才能保证相应的篇特换在需要的时候被正确选取（因为别名模板不能被特化）。
    C++ 标 准 库 中 也 提 供 了 与 之 相 应 的 类 型 萃 取 ： std::add_lvalue_reference<> 和
    std::add_rvalue_reference<>，在附录 D.4 中对它们有专门的介绍。该标准模板也包含了对 void
    类型的特化。
    
  移除限制符
    转换萃取可以分解或者引入任意种类的复合类型，并不仅限于引用。比如，如果一个类型中
    存在 const 限制符，我们可以将其移除：
    template<typename T>
    struct RemoveConstT {
    using Type = T;
    };
    template<typename T>
    struct RemoveConstT<T const> {
    using Type = T;
    };
    template<typename T>
    using RemoveConst = typename RemoveConstT<T>::Type;
    而且，转换萃取可以是多功能的，比如创建一个可以被用来移除 const 和 volatile 的
    RemoveCVT 萃取：
    #include "removeconst.hpp"
    #include "removevolatile.hpp"
    template<typename T>
    struct RemoveCVT : RemoveConstT<typename RemoveVolatileT<T>::Type>
    {
    };
    template<typename T>
    using RemoveCV = typename RemoveCVT<T>::Type;
    RemoveCVT 中有 两个 需要 注意 的地 方。 第一 个需 要注 意的 地方 是， 它同 时使 用了
    RemoveConstT 和相关的 RemoveVolitleT，首先移除类型中可能存在的 volatile，然后将得到
    了类型传递给 RemoveConstT。第二个需要注意的地方是，它没有定义自己的和 RemoveConstT
    中 Type 类似的成员，而是通过使用元函数转发（metafunction forwarding）从 RemoveConstT
    中继承了 Type 成员。这里元函数转发被用来简单的减少 RemoveCVT 中的类型成员。但是，
    即使是对于没有为所有输入都定义了元函数的情况，元函数转发也会很有用，在第 19.4 节
    中会进一步介绍这一技术。
    RemoveCVT 的别名模板可以被进一步简化成：
    template<typename T>
    using RemoveCV = RemoveConst<RemoveVolatile<T>>;
    同样地，这一简化只适用于 RemoveCVT 没有被特化的情况。但是和 AddLValueReference 以
    及 AddRValueReference 的情况不同的是，我们想不出一种对其进行特化的原因。
    C++ 标 准 库 也 提 供 了 与 之 对 应 的 std::remove_volatile<> ， std::remove_const<> ， 以 及
    std::remove_cv<>。在附录 D.4 中有对它们的讨论。
    
  退化（Decay）
    为了使对转换萃取的讨论变得更完整，我们接下来会实现一个模仿了按值传递参数时的类型
    转化行为的萃取。该类型转换继承自 C 语言，这意味着参数类型会发生退化（数组类型退化
    成指针类型，函数类型退化成指向函数的指针类型），而且会删除相应的顶层 const，volatile
    以及引用限制符（因为在解析一个函数调用时，会会忽略掉参数类型中的顶层限制符）。
    下面的程序展现了按值传递的效果，它会打印出经过编译器退化之后的参数类型：
    #include <iostream>
    #include <typeinfo>
    #include <type_traits>
    template<typename T>
    void f(T)
    {}
    template<typename A>
    void printParameterType(void (*)(A))
    {
    std::cout << "Parameter type: " << typeid(A).name() << ’\n’;
    std::cout << "- is int: " <<std::is_same<A,int>::value << ’\n’;
    std::cout << "- is const: " <<std::is_const<A>::value << ’\n’;
    std::cout << "- is pointer: " <<std::is_pointer<A>::value << ’\n’;
    }
    int main()
    {
    printParameterType(&f<int>);
    printParameterType(&f<int const>);
    printParameterType(&f<int[7]>);
    printParameterType(&f<int(int)>);
    }
    在程序的输出中，除了 int 参数保持不变外，其余 int const，int[7]，以及 int(int)参数分别退
    化成了 int，int*，以及 int(*)(int)。
    我们可以实现一个与之功能类似的萃取。为了和 C++标准库中的 std::decay 保持匹配，我们
    称之为 DecayT。它的实现结合了上文中介绍的多种技术。首先我们对非数组、非函数的情
    况进行定义，该情况只需要删除 const 和 volatile 限制符即可：
    template<typename T>
    struct DecayT : RemoveCVT<T>
    {
    };
    然后我们处理数组到指针的退化，这需要用偏特化来处理所有的数组类型（有界和无界数
    组）：
    template<typename T>
    struct DecayT<T[]> {
    using Type = T*;
    };
    template<typename T, std::size_t N>
    struct DecayT<T[N]> {
    using Type = T*;
    };
    最后来处理函数到指针的退化，这需要应对所有的函数类型，不管是什么返回类型以及有多
    数参数。为此，我们适用了变参模板：
    template<typename R, typename… Args>
    struct DecayT<R(Args…)> {
    using Type = R (*)(Args…);
    };
    template<typename R, typename… Args>
    struct DecayT<R(Args…, …)> {
    using Type = R (*)(Args…, …);
    };
    注意，上面第二个偏特化可以匹配任意使用了 C-style 可变参数的函数。下面的例子展示了
    DecayT 主模板以及其全部四种偏特化的使用：
    #include <iostream>
    #include <typeinfo>
    #include <type_traits>
    #include "decay.hpp"
    template<typename T>
    void printDecayedType()
    {
    using A = typename DecayT<T>::Type;
    std::cout << "Parameter type: " << typeid(A).name() << ’\n’;
    std::cout << "- is int: " << std::is_same<A,int>::value << ’\n’;
    std::cout << "- is const: " << std::is_const<A>::value << ’\n’;
    std::cout << "- is pointer: " << std::is_pointer<A>::value << ’\n’;
    }
    int main()
    {
    printDecayedType<int>();
    printDecayedType<int const>();
    printDecayedType<int[7]>();
    printDecayedType<int(int)>();
    }
    
    和往常一样，我们也提供了一个很方便的别名模板：
    template typename T>
    using Decay = typename DecayT<T>::Type;
    
    C++标准库也提供了相应的类型萃取 std::decay<>，在附录 D.4 中有相应的介绍。
     */
    namespace case2 {
        template<typename T>
        struct RemoveReferenceT {
            using Type = T;
        };
        template<typename T>
        struct RemoveReferenceT<T&> {
            using Type = T;
        };
        template<typename T>
        struct RemoveReferenceT<T&&> {
            using Type = T;
        };
        
        template<typename T>
        using RemoveReference = typename RemoveReferenceT<T>::Type;
    
        template<typename T>
        struct RemoveConstT {
            using Type = T;
        };
        template<typename T>
        struct RemoveConstT<T const> {
            using Type = T;
        };
        template<typename T>
        using RemoveConst = typename RemoveConstT<T>::Type;
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
//main_Type_Function_19_3()
{

    return 0;
}