//
// Created by yangfeng on 2022/8/2.
//
#include "../headers.h"

/*
萃取还是策略以及策略类（Traits versus Policiesand Policies Classes）
    到目前为止我们并没有区分累积（accumulation）和求和（summation）。但是我们也可以
    相像其它种类的累积。比如，我们可以对一组数值求积。或者说，如果这些值是字符串的话，
    我们可以将它们连接起来。即使是求一个序列中最大值的问题，也可以转化成一个累积问题。
    在所有这些例子中，唯一需要变得的操作是 accum()中的 total += *beg。我们可以称这一操
    作为累积操作的一个策略（policy）。
    下面是一个在 accum()中引入这样一个策略的例子：
    #ifndef ACCUM_HPP
    #define ACCUM_HPP
    #include "accumtraits4.hpp"
    #include "sumpolicy1.hpp"
    template<typename T,
    typename Policy = SumPolicy,
    typename Traits = AccumulationTraits<T>>
    auto accum (T const* beg, T const* end)
    {
    using AccT = typename Traits::AccT;
    AccT total = Traits::zero();
    while (beg != end) {
    Policy::accumulate(total, *beg);
    ++beg;
    }
    return total;
    }
    #endif //ACCUM_HPP
    在这一版的 accum()中，SumPolicy 是一个策略类，也就是一个通过预先商定好的接口，为算
    法实现了一个或多个策略的类。SumPolicy 可以被实现成下面这样：
    #ifndef SUMPOLICY_HPP
    #define SUMPOLICY_HPP
    class SumPolicy {
    public:
    template<typename T1, typename T2>
    static void accumulate (T1& total, T2 const& value) {
    total += value;
    }
    };
    #endif //SUMPOLICY_HPP
    如果提供一个不同的策略对数值进行累积的话，我们可以计算完全不同的事情。比如考虑下
    面这个程序，它试图计算一组数值的乘积：
    #include "accum6.hpp"
    #include <iostream>
    class MultPolicy {
    public:
    template<typename T1, typename T2>
    static void accumulate (T1& total, T2 const& value) {
    total *= value;
    }
    };
    int main()
    {
    // create array of 5 integer values
    int num[] = { 1, 2, 3, 4, 5 };
    // print product of all values
    std::cout << "the product of the integer values is " <<
    accum<int,MultPolicy>(num, num+5) << ’\n’;
    }
    但是这个程序的输出却和我们所期望的有所不同：
    the product of the integer values is 0
    问题出在我们对初始值的选取：虽然 0 能很好的满足求和的需求，但是却不适用于求乘积（初
    始值 0 会让乘积的结果也是 0）。这说明不同的萃取和策略可能会相互影响，也恰好强调了
    仔细设计模板的重要性。
    五车书馆
    184
    在这种情况下，我们可能会认识到，累积循环的初始值应该是累计策略的一部分。这个策略
    可以使用也可以不使用其 zero()萃取。其它一些方法也应该被记住：不是所有的事情都要用
    萃取和策略才能够解决的。比如，C++标准库中的 std::accumulate()就将其初始值当作了第三
    个参数。
 */

using namespace std;

namespace ch19_2 {

    namespace case1 {
        template <typename T>
        struct AccumulationTraits;
        
        template <>
        struct AccumulationTraits<int> {
            using AccT = long long;
            static AccT const zero() {
                return 0;
            }
        };
        
        class SumPolicy {
        public:
            template<typename T1, typename T2>
            static void accumulate (T1& total, T2 const& value) {
                total += value;
            }
        };
    
        class MultPolicy {
        public:
            template<typename T1, typename T2>
            static void accumulate (T1& total, T2 const& value) {
                total *= value;
            }
        };
        
        template<typename T,
                typename Policy = SumPolicy,
                typename Traits = AccumulationTraits<T>>
        auto accum (T const* beg, T const* end)
        {
            using AccT = typename Traits::AccT;
            AccT total = Traits::zero();
            while (beg != end) {
                Policy::accumulate(total, *beg);
                ++beg;
            }
            return total;
        }
        
        /*
        我们对初始值的选取：
         虽然 0 能很好的满足求和的需求，但是却不适用于求乘积（初始值 0 会让乘积的结果也是 0）。
         这说明不同的萃取和策略可能会相互影响，也恰好强调了仔细设计模板的重要性。
         
        在这种情况下，我们可能会认识到，累积循环的初始值应该是累计策略的一部分。
         这个策略可以使用也可以不使用其 zero()萃取。
         其它一些方法也应该被记住：不是所有的事情都要用萃取和策略才能够解决的。
         比如，C++标准库中的 std::accumulate()就将其初始值当作了第三个参数。
         */
        void test_accum() {
            // create array of 5 integer values
            int num[] = { 1, 2, 3, 4, 5 };
            // print product of all values
            std::cout << "the product of the integer values is " <<
                      accum<int,MultPolicy>(num, num+5) << '\n';
        }
    }
    
    /*
萃取和策略：有什么区别？（Traits and Policies: What’s the Difference?）
    可以设计一个合适的例子来证明策略只是萃取的一个特例。相反地，也可以认为萃取只是编
    码了一个特定的策略。
    新的精简牛津词典（The New Shorter Oxford English Dictionary）有如下表述：
    
    萃取 ... 一个为物体所特有的属性（a distinctive feature characterizing a thing）。
    
    策略 ... 任何被作为有益因素或者权宜之计而采取的行动（any course of action adopted
    as advantageous or expedient）。
    基于此，我们倾向于对策略类这一名词的使用做如下限制：它们应该是一些编码了某种与其
    它模板参数大致独立的行为的类。这和 Alexandrescu 在 Modern C++ Design 中的表述是一致
    的：
    策略和萃取有很多相似之处，只是它们更侧重于行为，而不是类型。
    引入了萃取技术的 Nathan Myers 则建议使用如下更为开放的定义：
    萃取类：一个用来代替模板参数的类。作为一个类，它整合了有用的类型和常量；作为一
    个模板，它为实现一个可以解决所有软件问题的“额外的中间层”提供了方法。
    总体而言，我们更倾向于使用如下（稍微模糊的）定义：
    
    萃取代表的是一个模板参数的本质的、额外的属性。
    
    策略代表的是泛型函数和类型（通常都有其常用地默认值）的可以配置的行为。
    为了进一步阐明两者之间可能的差异，我们列出了如下和萃取有关的观察结果：
    
    萃取在被当作固定萃取（fixed traits）的时候会比较有用（比如，当其不是被作为模板
    参数传递的时候）。
    
    萃取参数通常都有很直观的默认参数（很少被重写，或者简单的说是不能被重写）。
    
    萃取参数倾向于紧密的依赖于一个或者多个主模板参数。
    
    萃取在大多数情况下会将类型和常量结合在一起，而不是成员函数。
    
    萃取倾向于被汇集在萃取模板中。
    对于策略类，我们有如下观察结果：
    
    策略类如果不是被作为模板参数传递的话，那么其作用会很微弱。
    
    策略参数不需要有默认值，它们通常是被显式指定的（虽有有些泛型组件通常会使用默
    认策略）。
    
    策略参数通常是和其它模板参数无关的。
    五车书馆
    185
    
    策略类通常会包含成员函数。
    
    策略可以被包含在简单类或者类模板中。
    但是，两者之间并没有一个清晰的界限。比如，C++标准库中的字符萃取就定义了一些函数
    行为（比如比较，移动和查找字符）。通过替换这些萃取，我们定义一个大小写敏感的字符
    类型，同时又可以保留相同的字符类型。因此，虽然它们被称为萃取，但是它们的一些属性
    和策略确实有联系的。
     */
    
    namespace case2 {
    }
    
    /*
成员模板还是模板模板参数？（Member Templates versus Template Template Parameters）
    为了实现累积策略（accumulation policy），我们选择将 SumPolicy 和 MultPolicy 实现为有成
    员模板的常规类。另一种使用类模板设计策略类接口的方式，此时就可以被当作模板模板参
    数使用（template template arguments，参见第 5.7 节和第 12.2.3 节）。比如，我们可以将
    SumPolicy 重写为如下模板：
    #ifndef SUMPOLICY_HPP
    #define SUMPOLICY_HPP
    template<typename T1, typename T2>
    class SumPolicy {
    public:
    static void accumulate (T1& total, T2 const& value) {
    total += value;
    }
    };
    #endif //SUMPOLICY_HPP
    此时就可以调整 Accum，让其使用一个模板模板参数：
    #ifndef ACCUM_HPP#define ACCUM_HPP
    #include "accumtraits4.hpp"
    #include "sumpolicy2.hpp"
    template<typename T,
    template<typename,typename> class Policy = SumPolicy,
    typename Traits = AccumulationTraits<T>>
    auto accum (T const* beg, T const* end)
    {
    using AccT = typename Traits::AccT;
    AccT total = Traits::zero();
    while (beg != end) {
    Policy<AccT,T>::accumulate(total, *beg);
    ++beg;
    }
    return total;
    五车书馆
    186
    }
    #endif//ACCUM_HPP
    相同的转化也可以被用于萃取参数。（这一主题的其它变体也是有可能的：比如，相比于显
    式的将 Acct 的类型传递给策略类型，传递累积萃取并通过策略从萃取参数中获取类型，可
    能会更有优势一些。）
    通过模板模板参数访问策略类的主要优势是，让一个策略类通过一个依赖于模板参数的类型
    携带一些状态信息会更容易一些（比如 static 数据成员）。（在我们的第一个方法中，static
    数据成员必须要被嵌入到一个成员类模板中。）
    但是，模板模板参数方法的一个缺点是，策略类必须被实现为模板，而且模板参数必须和我
    们的接口所定义的参数一样。这可能会使萃取本身的表达相比于非模板类变得更繁琐，也更
    不自然
     */
    
    namespace case3 {
        template <typename T>
        struct AccumulationTraits;
    
        template <>
        struct AccumulationTraits<int> {
            using AccT = long long;
            static AccT const zero() {
                return 0;
            }
        };
    
        template <typename T1, typename T2>
        class SumPolicy {
        public:
            static void accumulate (T1& total, T2 const& value) {
                total += value;
            }
        };
    
        class MultPolicy {
        public:
            template<typename T1, typename T2>
            static void accumulate (T1& total, T2 const& value) {
                total *= value;
            }
        };
    
        template<typename T,
                template<typename,typename> class Policy = SumPolicy,
                typename Traits = AccumulationTraits<T>>
        auto accum (T const* beg, T const* end)
        {
            using AccT = typename Traits::AccT;
            AccT total = Traits::zero();
            while (beg != end) {
                Policy<AccT,T>::accumulate(total, *beg);
                ++beg;
            }
            return total;
        }
        
        void test_accum() {
            // create array of 5 integer values
            int num[] = { 1, 2, 3, 4, 5 };
            // print product of all values
            std::cout << "the product of the integer values is " <<
                      accum<int>(num, num+5) << '\n';
        }
    }
    
    /*
结合多个策略以及/或者萃取（Combining MultiplePolicies and/or Traits）
    正如我们的实现所展现的那样，萃取以及策略并不会完全摒除多模板参数的情况。但是，它
    们确实将萃取和模板的数量降低到了易于管理的水平。然后就有了一个很有意思的问题，该
    如何给这些模板参数排序？
    一个简单的策略是，根据参数默认值被选择的可能型进行递增排序（也就是说，越是有可能
    使用一个参数的默认值，就将其排的越靠后）。比如说，萃取参数通常要在策略参数后面，
    因为在客户代码中，策略更可能被重写。（善于观察的读者应该已经在我们的代码中发现了
    这一规律。）
    如果我们不介意增加代码的复杂性的话，还有一种可以按照任意顺序指定非默认参数的方
    法。具体请参见第 21.4 节。
     */
    
    namespace case4 {
    }
    
    /*
通过普通迭代器实现累积（Accumulation with General Iterators）
    在结束萃取和策略的介绍之前，最好再看下另一个版本的 accum()的实现，在该实现中添加
    了处理泛化迭代器的能力（不再只是简单的指针），这是为了支持工业级的泛型组件。有意
    思的是，我们依然可以用指针来调用这一实现，因为 C++标准库提供了迭代器萃取。此时我
    们就可以像下面这样定义我们最初版本的 accum()了（请暂时忽略后面的优化）：
    #ifndef ACCUM_HPP
    #define ACCUM_HPP
    五车书馆
    187
    #include <iterator>
    template<typename Iter>
    auto accum (Iter start, Iter end)
    {
    using VT = typename std::iterator_traits<Iter>::value_type;
    VT total{}; // assume this actually creates a zero value
    while (start != end) {
    total += *start;
    ++start;
    }
    return total;
    }
    #endif //ACCUM_HPP
    这里的 std::iterator_traits 包含了所有迭代器相关的属性。由于存在一个针对指针的偏特化，
    这些萃取可以很方便的被用于任意常规的指针类型。标准库对这一特性的支持可能会像下面
    这样：
    namespace std {
    template<typename T>
    struct iterator_traits<T*> {
    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = random_access_iterator_tag ;
    };
    }
    但是，此时并没有一个适用于迭代器所指向的数值的累积的类型；因此我们依然需要设计自
    己的 AccumulationTraits。
     */
    
    namespace case5 {
    
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_Traits_versus_Policies_and_Policy_Classes_19_2()
{

    return 0;
}
