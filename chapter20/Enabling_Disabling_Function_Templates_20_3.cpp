//
// Created by yangfeng on 2022/8/12.
//

#include "../headers.h"

using namespace std;
/*
20.3 Enable/Disable 函数模板
    算法特化需要提供可以基于模板参数的属性进行选择的、不同的函数模板。不幸的是，无论
    是函数模板的部分排序规则（参见 16.2.2 节）还是重载解析（参见附录 C），都不能满足更
    为高阶的算法特化的要求。
    C++标准库为之提供的一个辅助工具是 std::enable_if，我们曾在第 6.3 节对其进行了介绍。
    本节将介绍通过引入一个对应的模板别名，实现该辅助工具的方式，为了避免名称冲突，我
    们将称之称为 EnableIf。
    和 std::enable_if 一样，EnableIf 模板别名也可以被用来基于特定的条件 enable(或 disable)特
    定的函数模板。比如，随机访问版本的 advanceIter()算法可以被实现成这样：
    template<typename Iterator>
    constexpr bool IsRandomAccessIterator =
    IsConvertible< typename
    std::iterator_traits<Iterator>::iterator_category,
    std::random_access_iterator_tag>;
    template<typename Iterator, typename Distance>
    EnableIf<IsRandomAccessIterator<Iterator>>
    advanceIter(Iterator& x, Distance n){
    x += n; // constant time
    }
    五车书馆
    249
    这里使用了基于 EnableIf 的偏特化，在迭代器是随机访问迭代器的时候启用特定的
    advanceIter()变体。EnableIf 包含两个参数，一个是标示着该模板是否应该被启用的 bool 型
    条件参数，另一个是在第一个参数为 true 时，EnableIf 应该包含的类型。在我们上面的例子
    中，用在第 19.5 节和第 19.7.3 节介绍的 IsConvertible 类型萃取定义了一个新的类型萃取
    IsRandomAccessIterator。这样，这一特殊版本的 advanceIter()实现只有在模板参数 Iterator
    是被一个随机访问迭代器替换的时候才会被启用。
    EnableIf 的实现非常简单：
    template<bool, typename T = void>
    struct EnableIfT {
    };
    template< typename T>
    struct EnableIfT<true, T> {
    using Type = T;
    };
    template<bool Cond, typename T = void>
    using EnableIf = typename EnableIfT<Cond, T>::Type;
    EnableIf 会扩展成一个类型，因此它被实现成了一个别名模板（alias template）。我们希望
    为之使用偏特化（参见第 16 章），但是别名模板（alias template）并不能被偏特化。幸运
    的是，我们可以引入一个辅助类模板（helper class template）EnableIfT，并将真正要做的工
    作委托给它，而别名模板 EnableIf 所要做的只是简单的从辅助模板中选择结果类型。当条件
    是 true 的时候，EnableIfT<...>::Type（也就是 EnableIf<...>）的计算结果将是第二个模板参数
    T。当条件是 false 的时候，EnableIf 不会生成有效的类型，因为主模板 EnableIfT 没有名为 Type
    的成员。通常这应该是一个错误，但是在 SFINAE（参见第 15.7 节）上下文中（比如函数模
    板的返回类型），它只会导致模板参数推断失败，并将函数模板从待选项中移除。
    对于 advanceIter()，EnableIf 的使用意味着只有当 Iterator 参数是随机访问迭代器的时候，函
    数模板才可以被使用（而且返回类型是 void），而当 Iterator 不是随机访问迭代器的时候，
    函数模板则会被从待选项中移除。我们可以将 EnableIf 理解成一种在模板参数不满足特定需
    求的时候，防止模板被实例化的防卫手段。由于 advanceIter()需要一些只有随机访问迭代器
    才有操作，因此只能被随机访问迭代器实例化。有时候这样使用 EnableIf 也不是绝对安全的
    ----用户可能会断言一个类型是随机访问迭代器，却又没有为之提供相应的操作----此时
    EnableIf 可以被用来帮助尽早的发现这一类错误。
    现在我们已经可以显式的为特定的类型激活其所适用的更为特化的模板了。但是这还不够：
    我们还需要“去激活（de-activate）”不够特化的模板，因为在两个模板都适用的时候，编
    译期没有办法在两者之间做决断（order），从而会报出一个模板歧义错误。幸运的是，实
    现这一目的方法并不复杂：我们为不够特化的模板使用相同模式的 EnableIf，只是适用相反
    的判断条件。这样，就可以确保对于任意 Iterator 类型，都只有一个模板会被激活。因此，
    适用于非随机访问迭代器的 advanceIter()会变成下面这样：
    五车书馆
    250
    template<typename Iterator, typename Distance>
    EnableIf<!IsRandomAccessIterator<Iterator>>
    advanceIter(Iterator& x, Distance n)
    {
    while (n > 0) {//linear time
    ++x;
    --n;
    }
    }
    20.3.1 提供多种特化版本
    上述模式可以被继续泛化以满足有两种以上待选项的情况：可以为每一个待选项都配备一个
    EnableIf，并且让它们的条件部分，对于特定的模板参数彼此互斥。这些条件部分通常会用
    到多种可以用类型萃取（type traits）表达的属性。
    比如，考虑另外一种情况，第三种 advanceIter()算法的变体：允许指定一个负的距离参数，
    以让迭代器向“后”移动。很显然这对一个“输入迭代器（input itertor）”是不适用的，对
    一个随机访问迭代器却是适用的。但是，标准库也包含一种双向迭代器（bidirectional iterator）
    的概念，这一类迭代器可以向后移动，但却不要求必须同时是随机访问迭代器。实现这一情
    况需要稍微复杂一些的逻辑：每个函数模板都必须使用一个包含了在所有函数模板间彼此互
    斥 EnableIf 条件，这些函数模板代表了同一个算法的不同变体。这样就会有下面一组条件：
    
    随机访问迭代器：适用于随机访问的情况（常数时间复杂度，可以向前或向后移动）
    
    双向迭代器但又不是随机访问迭代器：适用于双向情况（线性时间复杂度，可以向前或
    向后移动）
    
    输入迭代器但又不是双向迭代器：适用于一般情况（线性时间复杂度，只能向前移动）
    相关函数模板的具体实现如下：
    #include <iterator>
    // implementation for random access iterators:
    template<typename Iterator, typename Distance>
    EnableIf<IsRandomAccessIterator<Iterator>>
    advanceIter(Iterator& x, Distance n) {
    x += n; // constant time
    }
    template<typename Iterator>
    constexpr bool IsBidirectionalIterator =
    IsConvertible< typename
    std::iterator_traits<Iterator>::iterator_category,
    std::bidirectional_iterator_tag>;
    // implementation for bidirectional iterators:
    五车书馆
    251
    template<typename Iterator, typename Distance>
    EnableIf<IsBidirectionalIterator<Iterator>
    && !IsRandomAccessIterator<Iterator>>
    advanceIter(Iterator& x, Distance n) {
    if (n > 0) {
    for ( ; n > 0; ++x, --n) { //linear time
    }
    } else {
    for ( ; n < 0; --x, ++n) { //linear time
    }
    }
    }
    // implementation for all other iterators:
    template<typename Iterator, typename Distance>
    EnableIf<!IsBidirectionalIterator<Iterator>>
    advanceIter(Iterator& x, Distance n) {
    if (n < 0) {
    throw "advanceIter(): invalid iterator category for negative n";
    }
    while (n > 0) { //linear time
    ++x;
    --n;
    }
    }
    通过让每一个函数模板的 EnableIf 条件与其它所有函数模板的条件互相排斥，可以保证对于
    一组参数，最多只有一个函数模板可以在模板参数推断中胜出。
    上述例子已体现出通过 EnableIf 实现算法特化的一个缺点：每当一个新的算法变体被加入进
    来，就需要调整所有算法变体的 EnableIf 条件，以使得它们之间彼此互斥。作为对比，当通
    过标记派发（tag dispatching）引入一个双向迭代器的算法变体时，则只需要使用标记
    std::bidirectional_iterator_tag 重载一个 advanceIterImpl()即可。
    标记派发（tag dispatching）和 EnableIf 两种技术所适用的场景有所不同：一般而言，标记派
    发可以基于分层的 tags 支持简单的派发，而 EnableIf 则可以基于通过使用类型萃取（type
    trait）获得的任意一组属性来支持更为复杂的派发。
    20.3.2 EnableIf 所之何处（where does the EnableIf Go）?
    EnableIf 通常被用于函数模板的返回类型。但是，该方法不适用于构造函数模板以及类型转
    换模板，因为它们都没有被指定返回类型。而且，使用 EnableIf 也会使得返回类型很难被读
    懂。对于这一问题，我们可以通过将 EnableIf 嵌入一个默认的模板参数来解决，比如：
    五车书馆
    252
    #include <iterator>
    #include "enableif.hpp"
    #include "isconvertible.hpp"
    template<typename Iterator>
    constexpr bool IsInputIterator = IsConvertible< typename
    std::iterator_traits<Iterator>::iterator_category,
    std::input_iterator_tag>;
    template<typename T>
    class Container {
    public:
    // construct from an input iterator sequence:
    template<typename Iterator, typename =
    EnableIf<IsInputIterator<Iterator>>>
    Container(Iterator first, Iterator last);
    // convert to a container so long as the value types are convertible:
    template<typename U, typename = EnableIf<IsConvertible<T, U>>>
    operator Container<U>() const;
    };
    但是，这样做也有一个问题。如果我们尝试再添加一个版本的重载的话，会导致错误：
    // construct from an input iterator sequence:
    template<typename Iterator,
    typename = EnableIf<IsInputIterator<Iterator>
    && !IsRandomAccessIterator<Iterator>>>
    Container(Iterator first, Iterator last);
    template<typename Iterator, typename =
    EnableIf<IsRandomAccessIterator<Iterator>>>
    Container(Iterator first, Iterator last); // ERROR: redeclaration //
    of constructor template
    问题在于这两个模板唯一的区别是默认模板参数，但是在判断两个模板是否相同的时候却又
    不会考虑默认模板参数。
    该问题可以通过引入另外一个模板参数来解决，这样两个构造函数模板就有数量不同的模板
    参数了：
    // construct from an input iterator sequence:
    template<typename Iterator, typename =
    EnableIf<IsInputIterator<Iterator>
    && !IsRandomAccessIterator<Iterator>>>
    Container(Iterator first, Iterator last);
    template<typename Iterator, typename =
    五车书馆
    253
    EnableIf<IsRandomAccessIterator<Iterator>>, typename = int> // extra
    dummy parameter to enable both constructors
    Container(Iterator first, Iterator last); //OK now
    20.3.3 编译期 if
    值得注意的是，C++17 的 constexpr if 特性（参见第 8.5 节）使得某些情况下可以不再使用
    EnableIf。比如在 C++17 中可以像下面这样重写 advanceIter():
    template<typename Iterator, typename Distance>
    void advanceIter(Iterator& x, Distance n) {
    if constexpr(IsRandomAccessIterator<Iterator>) {
    // implementation for random access iterators:
    x += n; // constant time
    }else if constexpr(IsBidirectionalIterator<Iterator>) {
    // implementation for bidirectional iterators:
    if (n > 0)
    {for ( ; n > 0; ++x, --n) { //linear time for positive n
    }
    } else {
    for ( ; n < 0; --x, ++n) { //linear time for negative n
    }
    }
    }else {
    // implementation for all other iterators that are at least input iterators:
    if (n < 0) {
    throw "advanceIter(): invalid iterator category for negative n";
    }
    while (n > 0) { //linear time for positive n only
    ++x;
    --n;
    }
    }
    }
    这样会更好一些。更为特化的代码分支只会被那些支持它们的类型实例化。因此，对于使用
    了不被所有的迭代器都支持的代码的情况，只要它们被放在合适的 constexpr if 分支中，就
    是安全的。
    但是，该方法也有其缺点。只有在泛型代码组件可以被在一个函数模板中完整的表述时，这
    一使用 constexpr if 的方法才是可能的。在下面这些情况下，我们依然需要 EnableIf：
    
    需要满足不同的“接口”需求
    
    需要不同的 class 定义
    
    对于某些模板参数列表，不应该存在有效的实例化。
    五车书馆
    254
    对于最后一种情况，下面这种做法看上去很有吸引力：
    template<typename T>
    void f(T p) {
    if constexpr (condition<T>::value) {
    // do something here…
    }
    else {
    // not a T for which f() makes sense:
    static_assert(condition<T>::value, "can’t call f() for such a T");
    }
    }
    但是我们并不建议这样做，因为它对 SFINAE 不太友好：函数 f<T>()并不会被从待选项列表中
    移除，因此它有可能会屏蔽掉另一种重载解析结果。作为对比，使用 EnableIf f<T>()则会在
    EnableIf<...>替换失败的时候将该函数从待选项列表中移除。
    20.3.4 Concepts
    上述技术到目前为止都还不错，但是有时候却稍显笨拙，它们可能会占用很多的编译器资源，
    以及在某些情况下，可能会产生难以理解的错误信息。因此某些泛型库的作者一直都在盼望
    着一种能够更简单、直接地实现相同效果的语言特性。为了满足这一需求，一个被称为
    conceptes 的特性很可能会被加入到 C++语言中；具体请参见第 6.5 节，第 18.4 节以及附录 E。
    比如，我们可能希望被重载的 container 的构造函数可以像下面这样：
    template<typename T>
    class Container {
    public:
    //construct from an input iterator sequence:
    template<typename Iterator>
    requires IsInputIterator<Iterator>
    Container(Iterator first, Iterator last);
    // construct from a random access iterator sequence:
    template<typename Iterator>
    requires IsRandomAccessIterator<Iterator>
    Container(Iterator first, Iterator last);
    // convert to a container so long as the value types are convertible:
    template<typename U>
    requires IsConvertible<T, U>
    operator Container<U>() const;
    };
    五车书馆
    255
    其中 requires 条款（参见第 E.1 节）描述了使用当前模板的要求。如果某个要求不被满足，
    那么相应的模板就不会被当作备选项考虑。因此它可以被当作 EnableIf 这一想法的更为直接
    的表达方式，而且是被语言自身支持的。
    Requires 条款还有另外一些优于 EnableIf 的地方。约束包容（constraint subsumption，参见
    第 E.3.1 节）为只有 requires 不同的模板进行了排序，这样就不再需要标记派发了（tag
    dispatching）。而且，requires 条款也可以被用于非模板。比如只有在 T 的对象可以被<运算
    符比较的时候，才为容器提供 sort()成员函数：
    template<typename T>
    class Container {
    public:
    …
    requires HasLess<T>
    void sort() {
    …
    }
    };
 */

namespace ch20_3 {

    namespace case1 {
        /*
        
        template<typename Iterator>
        constexpr bool IsRandomAccessIterator =
                IsConvertible< typename
                std::iterator_traits<Iterator>::iterator_category,
                        std::random_access_iterator_tag>;
        
        template<typename Iterator, typename Distance>
        EnableIf<IsRandomAccessIterator<Iterator>>
        advanceIter(Iterator& x, Distance n){
            x += n; // constant time
        }
         
         */
    
        template<bool, typename T = void>
        struct EnableIfT {
        };
    
        template<typename T>
        struct EnableIfT<true, T> {
            using Type = T;
        };
    
        template<bool Cond, typename T = void>
        using EnableIf = typename EnableIfT<Cond, T>::Type;
    
        /*
        现在我们已经可以显式的为特定的类型激活其所适用的更为特化的模板了。但是这还不够：
            我们还需要“去激活（de-activate）”不够特化的模板，因为在两个模板都适用的时候，编
            译期没有办法在两者之间做决断（order），从而会报出一个模板歧义错误。幸运的是，实
            现这一目的方法并不复杂：我们为不够特化的模板使用相同模式的 EnableIf，只是适用相反
            的判断条件。这样，就可以确保对于任意 Iterator 类型，都只有一个模板会被激活。因此，
            适用于非随机访问迭代器的 advanceIter()会变成下面这样：
         */
    
    
        /*
        template<typename Iterator, typename Distance>
        EnableIf<!IsRandomAccessIterator<Iterator>>
        advanceIter(Iterator& x, Distance n)
        {
            while (n > 0) {//linear time
            ++x;
            --n;
        }
         */
    }
    
    
    namespace case2 {
        /*
        20.3.1 提供多种特化版本
            上述模式可以被继续泛化以满足有两种以上待选项的情况：可以为每一个待选项都配备一个
            EnableIf，并且让它们的条件部分，对于特定的模板参数彼此互斥。这些条件部分通常会用
            到多种可以用类型萃取（type traits）表达的属性。
            比如，考虑另外一种情况，第三种 advanceIter()算法的变体：允许指定一个负的距离参数，
            以让迭代器向“后”移动。很显然这对一个“输入迭代器（input itertor）”是不适用的，对
            一个随机访问迭代器却是适用的。但是，标准库也包含一种双向迭代器（bidirectional iterator）
            的概念，这一类迭代器可以向后移动，但却不要求必须同时是随机访问迭代器。实现这一情
            况需要稍微复杂一些的逻辑：每个函数模板都必须使用一个包含了在所有函数模板间彼此互
            斥 EnableIf 条件，这些函数模板代表了同一个算法的不同变体。这样就会有下面一组条件：
            
            随机访问迭代器：适用于随机访问的情况（常数时间复杂度，可以向前或向后移动）
            
            双向迭代器但又不是随机访问迭代器：适用于双向情况（线性时间复杂度，可以向前或
            向后移动）
            
            输入迭代器但又不是双向迭代器：适用于一般情况（线性时间复杂度，只能向前移动）
            相关函数模板的具体实现如下：
            #include <iterator>
            // implementation for random access iterators:
            template<typename Iterator, typename Distance>
            EnableIf<IsRandomAccessIterator<Iterator>>
            advanceIter(Iterator& x, Distance n) {
            x += n; // constant time
            }
            template<typename Iterator>
            constexpr bool IsBidirectionalIterator =
            IsConvertible< typename
            std::iterator_traits<Iterator>::iterator_category,
            std::bidirectional_iterator_tag>;
            // implementation for bidirectional iterators:
            五车书馆
            251
            template<typename Iterator, typename Distance>
            EnableIf<IsBidirectionalIterator<Iterator>
            && !IsRandomAccessIterator<Iterator>>
            advanceIter(Iterator& x, Distance n) {
            if (n > 0) {
            for ( ; n > 0; ++x, --n) { //linear time
            }
            } else {
            for ( ; n < 0; --x, ++n) { //linear time
            }
            }
            }
            // implementation for all other iterators:
            template<typename Iterator, typename Distance>
            EnableIf<!IsBidirectionalIterator<Iterator>>
            advanceIter(Iterator& x, Distance n) {
            if (n < 0) {
            throw "advanceIter(): invalid iterator category for negative n";
            }
            while (n > 0) { //linear time
            ++x;
            --n;
            }
            }
            通过让每一个函数模板的 EnableIf 条件与其它所有函数模板的条件互相排斥，可以保证对于
            一组参数，最多只有一个函数模板可以在模板参数推断中胜出。
            上述例子已体现出通过 EnableIf 实现算法特化的一个缺点：每当一个新的算法变体被加入进
            来，就需要调整所有算法变体的 EnableIf 条件，以使得它们之间彼此互斥。作为对比，当通
            过标记派发（tag dispatching）引入一个双向迭代器的算法变体时，则只需要使用标记
            std::bidirectional_iterator_tag 重载一个 advanceIterImpl()即可。
            标记派发（tag dispatching）和 EnableIf 两种技术所适用的场景有所不同：一般而言，标记派
            发可以基于分层的 tags 支持简单的派发，而 EnableIf 则可以基于通过使用类型萃取（type
            trait）获得的任意一组属性来支持更为复杂的派发。
         */
        
        
        
    }
    
    namespace case3 {
        /*
        20.3.2 EnableIf 所之何处（where does the EnableIf Go）?
EnableIf 通常被用于函数模板的返回类型。但是，该方法不适用于构造函数模板以及类型转
换模板，因为它们都没有被指定返回类型。而且，使用 EnableIf 也会使得返回类型很难被读
懂。对于这一问题，我们可以通过将 EnableIf 嵌入一个默认的模板参数来解决，比如：
         */
        
        
        
    }
    
    namespace case4 {
        /*
     20.3.3 编译期 if
        值得注意的是，C++17 的 constexpr if 特性（参见第 8.5 节）使得某些情况下可以不再使用
        EnableIf。比如在 C++17 中可以像下面这样重写 advanceIter():
        template<typename Iterator, typename Distance>
        void advanceIter(Iterator& x, Distance n) {
        if constexpr(IsRandomAccessIterator<Iterator>) {
        // implementation for random access iterators:
        x += n; // constant time
        }else if constexpr(IsBidirectionalIterator<Iterator>) {
        // implementation for bidirectional iterators:
        if (n > 0)
        {for ( ; n > 0; ++x, --n) { //linear time for positive n
        }
        } else {
        for ( ; n < 0; --x, ++n) { //linear time for negative n
        }
        }
        }else {
        // implementation for all other iterators that are at least input iterators:
        if (n < 0) {
        throw "advanceIter(): invalid iterator category for negative n";
        }
        while (n > 0) { //linear time for positive n only
        ++x;
        --n;
        }
        }
        }
        这样会更好一些。更为特化的代码分支只会被那些支持它们的类型实例化。因此，对于使用
        了不被所有的迭代器都支持的代码的情况，只要它们被放在合适的 constexpr if 分支中，就
        是安全的。
        但是，该方法也有其缺点。只有在泛型代码组件可以被在一个函数模板中完整的表述时，这
        一使用 constexpr if 的方法才是可能的。在下面这些情况下，我们依然需要 EnableIf：
        
        需要满足不同的“接口”需求
        
        需要不同的 class 定义
        
        对于某些模板参数列表，不应该存在有效的实例化。
        五车书馆
        254
        对于最后一种情况，下面这种做法看上去很有吸引力：
        template<typename T>
        void f(T p) {
        if constexpr (condition<T>::value) {
        // do something here…
        }
        else {
        // not a T for which f() makes sense:
        static_assert(condition<T>::value, "can’t call f() for such a T");
        }
        }
        但是我们并不建议这样做，因为它对 SFINAE 不太友好：函数 f<T>()并不会被从待选项列表中
        移除，因此它有可能会屏蔽掉另一种重载解析结果。作为对比，使用 EnableIf f<T>()则会在
        EnableIf<...>替换失败的时候将该函数从待选项列表中移除。
         */
        
        
        
    }
    
    namespace case5 {
        /*
        20.3.4 Concepts
        上述技术到目前为止都还不错，但是有时候却稍显笨拙，它们可能会占用很多的编译器资源，
        以及在某些情况下，可能会产生难以理解的错误信息。因此某些泛型库的作者一直都在盼望
        着一种能够更简单、直接地实现相同效果的语言特性。为了满足这一需求，一个被称为
        conceptes 的特性很可能会被加入到 C++语言中；具体请参见第 6.5 节，第 18.4 节以及附录 E。
        比如，我们可能希望被重载的 container 的构造函数可以像下面这样：
        template<typename T>
        class Container {
        public:
        //construct from an input iterator sequence:
        template<typename Iterator>
        requires IsInputIterator<Iterator>
        Container(Iterator first, Iterator last);
        // construct from a random access iterator sequence:
        template<typename Iterator>
        requires IsRandomAccessIterator<Iterator>
        Container(Iterator first, Iterator last);
        // convert to a container so long as the value types are convertible:
        template<typename U>
        requires IsConvertible<T, U>
        operator Container<U>() const;
        };
        五车书馆
        255
        其中 requires 条款（参见第 E.1 节）描述了使用当前模板的要求。如果某个要求不被满足，
        那么相应的模板就不会被当作备选项考虑。因此它可以被当作 EnableIf 这一想法的更为直接
        的表达方式，而且是被语言自身支持的。
        Requires 条款还有另外一些优于 EnableIf 的地方。约束包容（constraint subsumption，参见
        第 E.3.1 节）为只有 requires 不同的模板进行了排序，这样就不再需要标记派发了（tag
        dispatching）。而且，requires 条款也可以被用于非模板。比如只有在 T 的对象可以被<运算
        符比较的时候，才为容器提供 sort()成员函数：
        template<typename T>
        class Container {
        public:
        …
        requires HasLess<T>
        void sort() {
        …
        }
        };
         */
        
    }

    class Tmp {
    public:
    
    };

}

int
main()
//main_Enabling_Disabling_Function_Templates_20_3()
{

    return 0;
}