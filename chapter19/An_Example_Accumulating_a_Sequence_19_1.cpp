//
// Created by yangfeng on 2022/8/2.
//
#include "../headers.h"

/*
萃取的实现
模板允许我们用多种类型对类和函数进行参数化。如果能够通过引入尽可能多的模板参数，
去尽可能多的支持某种类型或者算法的各个方面，那将是一件很吸引人的事情。这样，我们
“模板化”的代码就可以被针对客户的某种具体需求进行实例化。但是从实际的角度来看，
我们并不希望引入过多的模板参数来实现最大化的参数化。因为让用户在客户代码中指出所
有的模板参数是一件很繁杂的事情，而且每增加一个额外的模板参数，都会使模板和客户代
码之间的协议变得更加复杂。

幸运的是，时间证明大部分被引入的额外的模板参数都有合理的默认值。在一些情况下，额
外的模板参数可以完全由很少的、主要的模板参数决定，我们接下来会看到，这一类额外的
模板参数可以被一起省略掉。在大多数情况下，其它一些参数的默认值可以从主模板参数得
到，但是默认值需要偶尔被重载（针对特殊应用）。当然也有一些参数和主模板参数无关：
在某种意义上，它们是其自身的主模板参数，只不过它们有适用于大多数情况的默认值。
萃取（或者叫萃取模板，traits/traits template）是 C++编程的组件，它们对管理那些在设计
工业级应用模板时所需要管理的多余参数很有帮助。在本章中，我们会展示一些可以证明该
类技术很有帮助的情况，同时也会展示各种各样的、可以让你写出更可靠也更为强大的工具
的技术。

本章所展示的大部分萃取技术在 C++标准库中都有其对应的存在。但是，为了更为清晰，我
们会将实现简化，删除只有在工业级实现中才会用到的细节。因此，我们会按照我们的命名
规则来命名这些技术，但是你应该可以很容易的把它们和标准库对应起来。
 */

using namespace std;

/*
19.1 一个例子：对一个序列求和
    计算一个序列中所有元素的和是一个很常规的任务。也正是这个简单的问题，给我们提供了
    一个很好的、可以用来介绍各种不同等级的萃取应用的例子。
    
19.1.1 固定的萃取（Fixed Traits）
    让我们先来考虑这样一种情况：待求和的数据存储在一个数组中，然后我们有一个指向数组
    中第一个元素的指针，和一个指向最后一个元素的指针。由于本书介绍的是模板，我们自然
    也希望写出一个适用于各种类型的模板。下面是一个看上去很直接的实现：
    #ifndef ACCUM_HPP
    #define ACCUM_HPP
    template<typename T>
    T accum (T const* beg, T const* end)
    {
    T total{}; // assume this actually creates a zero value
    while (beg != end) {
    total += *beg;
    ++beg;
    }
    return total;
    }
    #endif //ACCUM_HPP
    例子中唯一有些微妙的地方是，如何创建一个类型正确的零值（zero value）来作为求和的
    起始值。此处我们使用了在第 5.2 节介绍的值初始化（value initialization，用到了{...}符号）。
    这就意味着这个局部的 total 对象要么被其默认值初始化，要么被零（zero）初始化（对应
    指针是用 nullptr 初始化，对应 bool 值是用 false 初始化）。
    为了引入我们的第一个萃取模板，考虑下面这一个使用了 accum()的例子：
    #include "accum1.hpp"
    #include <iostream>
    int main()
    {
    // create array of 5 integer values
    int num[] = { 1, 2, 3, 4, 5 };
    // print average value
    std::cout << "the average value of the integer values is " << accum(num,
    num+5) / 5 << ’\n’;
    // create array of character values
    char name[] = "templates";
    int length = sizeof(name)-1;
    // (try to) print average character value
    std::cout << "the average value of the characters in \"" << name <<
    "\" is " << accum(name, name+length) / length << ’\n’;
    }
    在例子的前半部分，我们用 accum()对 5 个整型遍历求和：
    int num[] = { 1, 2, 3, 4, 5 };
    …
    accum(num0, num+5)
    接着就可以用这些变量的和除变量的数目得到平均值。
    例子的第二部分试图为单词“templates”中所有的字符做相同的事情。结果应该是 a 到 z
    之间的某一个值。在当今的大多数平台上，这个值都是通过 ASCII 码决定的：a 被编码成 97，
    z 被编码成 122。因此我们可能会期望能够得到一个介于 97 和 122 之间的返回值。但是在我
    们的平台上，程序的输出却是这样的：
    the average value of the integer values is 3
    the average value of the characters in "templates" is -5
    问题在于我们的模板是被 char 实例化的，其数值范围即使是被用来存储相对较小的数值的
    和也是不够的。很显然，为了解决这一问题我们应该引入一个额外的模板参数 AccT，并将
    其用于返回值 total 的类型。但是这会给模板的用户增加负担：在调用这一模板的时候，他
    们必须额外指定一个类型。对于上面的例子，我们可能需要将其写称这个样子：
    accum<int>(name,name+5)
    这并不是一个过于严苛的要求，但是确实是可以避免的。
    一个可以避免使用额外的模板参数的方式是，在每个被用来实例化 accum()的 T 和与之对应
    的应该被用来存储返回值的类型之间建立某种联系。这一联系可以被认为是 T 的某种属性。
    正如下面所展示的一样，可以通过模板的偏特化建立这种联系：
    template<typename T>
    struct AccumulationTraits;
    template<>
    struct AccumulationTraits<char> {
    using AccT = int;
    };
    template<>
    struct AccumulationTraits<short> {
    using AccT = int;
    };
    template<>
    struct AccumulationTraits<int> {
    using AccT = long;
    };
    template<>
    struct AccumulationTraits<unsigned int> {
    using AccT = unsigned long;
    };
    template<>
    struct AccumulationTraits<float> {
    using AccT = double;
    };
    AccumulationTraits 模板被称为萃取模板，因为它是提取了其参数类型的特性。（通常而言可
    以有不只一个萃取，也可以有不只一个参数）。我们选择不对这一模板进行泛型定义，因为
    在不了解一个类型的时候，我们无法为其求和的类型做出很好的选择。但是，可能有人会辩
    解说 T 类型本身就是最好的待选类型（很显然对于我们前面的例子不是这样）。
    有了这些了解之后，我们可以将 accum()按照下面的方式重写：
    
    #ifndef ACCUM_HPP
    #define ACCUM_HPP
    #include "accumtraits2.hpp"
    template<typename T>
    auto accum (T const* beg, T const* end)
    {
    // return type is traits of the element type
    using AccT = typename AccumulationTraits<T>::AccT;
    AccT total{}; // assume this actually creates a zero value
    while (beg != end) {
    total += *beg;
    ++beg;
    }
    return total;
    }
    #endif //ACCUM_HPP
    
    此时程序的输出就和我们所预期一样了：
    the average value of the integer values is 3
    the average value of the characters in "templates" is 108
    考虑到我们为算法加入了很好的检查机制，总体而言这些变化不算太大。而且，如果要将
    accum()用于新的类型的话，只要对 AccumulationTraits 再进行一次显式的偏特化，就会得到
    一个 AccT。值得注意的是，我们可以为任意类型进行上述操作：基础类型，声明在其它库
    中的类型，以及其它诸如此类的类型。
 */

namespace ch19_1 {

    namespace case1 {
        
        // 求数组元素的和
        template<typename T>
        T accum (T const* beg, T const* end)
        {
            T total{};                          // assume this actually creates a zero value
            while (beg != end) {
                total += *beg;
                ++beg;
            }
            return total;
        }
        
        void test_accum() {
            int num[] = {1, 2, 3, 4, 5};
            cout << accum(num, num + 5) << endl;
            char name[] = "templates";
            int len = sizeof(name) - 1;
            cout << accum(name, name + len) / len << endl;   // 这里出现了char类型大小装不下的问题
        }
        
        
        
    }
    
    namespace case2 {
    
        // AccumulationTraits 模板被称为萃取模板，因为它是提取了其参数类型的特性。
        //    （通常而言可以有不只一个萃取，也可以有不只一个参数）。
        template<typename T>
        struct AccumulationTraits;
        
        template<>
        struct AccumulationTraits<char> {
            using AccT = int;
        };
        
        template<>
        struct AccumulationTraits<short> {
            using AccT = int;
        };
        
        template<>
        struct AccumulationTraits<int> {
            using AccT = long;
        };
        
        template<>
        struct AccumulationTraits<unsigned int> {
            using AccT = unsigned long;
        };
        
        template<>
        struct AccumulationTraits<float> {
            using AccT = double;
        };
    
        template<typename T>
        auto accum (T const* beg, T const* end)
        {
            // return type is traits of the element type
            using AccT = typename AccumulationTraits<T>::AccT;
            AccT total{};                                           // assume this actually creates a zero value
            while (beg != end) {
                total += *beg;
                ++beg;
            }
            return total;
        }
        
        void test_accum() {
            int num[] = {1, 2, 3, 4, 5};
            cout << accum(num, num + 5) << endl;
            char name[] = "templates";
            int len = sizeof(name) - 1;
            cout << accum(name, name + len) / len << endl;   // 这里出现了char类型大小装不下的问题
        }
        
    }
    
    /*
值萃取（Value Traits）
    到目前为止我们看到的萃取，代表的都是特定“主”类型的额外的类型信息。在本节我们将
    会看到，这一“额外的信息”并不仅限于类型信息。还可以将常量以及其它数值类和一个类
    型关联起来。
    在最原始的 accum()模板中，我们使用默认构造函数对返回值进行了初始化，希望将其初始
    化为一个类似零（zero like）的值：
    AccT total{}; // assume this actually creates a zero value
    …
    return total;
    很显然，这并不能保证一定会生成一个合适的初始值。因为 AccT 可能根本就没有默认构造
    五车书馆
    178
    函数。
    萃取可以再一次被用来救场。对于我们的例子，我们可以为 AccumulationTraits 添加一个新
    的值萃取（value trait，似乎翻译成值特性会更好一些）：
    template<typename T>
    struct AccumulationTraits;
    template<>
    struct AccumulationTraits<char> {
    using AccT = int;
    static AccT const zero = 0;
    };
    template<>
    struct AccumulationTraits<short> {
    using AccT = int;
    static AccT const zero = 0;
    };
    template<>
    struct AccumulationTraits<int> {
    using AccT = long;
    static AccT const zero = 0;
    };
    …
    在这个例子中，新的萃取提供了一个可以在编译期间计算的，const的zero成员。此时，accum()
    的实现如下：
    #ifndef ACCUM_HPP
    #define ACCUM_HPP
    #include "accumtraits3.hpp"
    template<typename T>
    auto accum (T const* beg, T const* end)
    {
    // return type is traits of the element type
    using AccT = typename AccumulationTraits<T>::AccT;
    AccT total = AccumulationTraits<T>::zero; // init total by trait value
    while (beg != end) {
    total += *beg;
    ++beg;
    }
    return total;
    } #
    五车书馆
    179
    endif // ACCUM_HPP
    在上述代码中，存储求和结果的临时变量的初始化依然很直观：
    AccT total = AccumulationTraits<T>::zero;
    这一实现的一个不足之处是，C++只允许我们在类中对一个整形或者枚举类型的 static const
    数据成员进行初始化。
    Constexpr 的 static 数据成员会稍微好一些，允许我们对 float 类型以及其它字面值类型进行
    类内初始化：
    template<>
    struct AccumulationTraits<float> {
    using Acct = float;
    static constexpr float zero = 0.0f;
    };
    但是无论是 const 还是 constexpr 都禁止对非字面值类型进行这一类初始化。比如，一个用
    户定义的任意精度的 BigInt 类型，可能就不是字面值类型，因为它可能会需要将一部分信息
    存储在堆上（这会阻碍其成为一个字面值类型），或者是因为我们所需要的构造函数不是
    constexpr 的。下面这个实例化的例子就是错误的：
    class BigInt {
    BigInt(long long);
    …
    };
    …
    template<>
    struct AccumulationTraits<BigInt> {
    using AccT = BigInt;
    static constexpr BigInt zero = BigInt{0}; // ERROR: not a literal type
    构造函数不是 constexpr 的
    };
    一个比较直接的解决方案是，不再类中定义值萃取（只做声明）：
    template<>
    struct AccumulationTraits<BigInt> {
    using AccT = BigInt;
    static BigInt const zero; // declaration only
    };
    然后在源文件中对其进行初始化，像下面这样：
    BigInt const AccumulationTraits<BigInt>::zero = BigInt{0};
    五车书馆
    180
    这样虽然可以工作，但是却有些麻烦（必须在两个地方同时修改代码），这样可能还会有些
    低效，因为编译期通常并不知晓在其它文件中的变量定义。
    在 C++17 中，可以通过使用 inline 变量来解决这一问题：
    template<>
    struct AccumulationTraits<BigInt> {
    using AccT = BigInt;
    inline static BigInt const zero = BigInt{0}; // OK since C++17
    };
    在 C++17 之前的另一种解决办法是，对于那些不是总是生成整型值的值萃取，使用 inline 成
    员函数。同样的，如果成员函数返回的是字面值类型，可以将该函数声明为 constexpr 的。
    比如，我们可以像下面这样重写 AccumulationTraits：
    template<typename T>
    struct AccumulationTraits;
    template<>
    struct AccumulationTraits<char> {
    using AccT = int;
    static constexpr AccT zero() {
    return 0;
    }
    };
    template<>
    struct AccumulationTraits<short> {
    using AccT = int;
    static constexpr AccT zero() {
    return 0;
    }
    };
    template<>
    struct AccumulationTraits<int> {
    using AccT = long;
    static constexpr AccT zero() {
    return 0;
    }
    };
    template<>
    struct AccumulationTraits<unsigned int> {
    using AccT = unsigned long;
    五车书馆
    181
    static constexpr AccT zero() {
    return 0;
    }
    };
    template<>
    struct AccumulationTraits<float> {
    using AccT = double;
    static constexpr AccT zero() {
    return 0;
    }
    };
    …
    然后针我们自定义的类型对这些萃取进行扩展：
    template<>
    struct AccumulationTraits<BigInt> {
    using AccT = BigInt;
    static BigInt zero() {
    return BigInt{0};
    }
    };
    在应用端，唯一的区别是函数的调用语法（不像访问一个 static 数据成员那么简洁）：
    AccT total = AccumulationTraits<T>::zero(); // init total by trait
    function
    很明显，萃取可以不只是类型。在我们的例子中，萃取可以是一种能够提供所有在调用
    accum()时所需的调用参数的信息的技术。这是萃取这一概念的关键：萃取为泛型编程提供
    了一种配置（configure）具体元素（通常是类型）的手段。
     */
    
    namespace case3 {
        template<typename T>
        struct AccumulationTraits;
        template<>
        struct AccumulationTraits<char> {
            using AccT = int;
            static AccT const zero = 0;
        };
        template<>
        struct AccumulationTraits<short> {
            using AccT = int;
            static AccT const zero = 0;
        };
        template<>
        struct AccumulationTraits<int> {
            using AccT = long;
            static AccT const zero = 0;
        };
    
        template<>
        struct AccumulationTraits<unsigned int> {
            using AccT = unsigned long;
            static AccT const zero = 0;
        };
    
        template<>
        struct AccumulationTraits<float> {
            using AccT = double;
            // Constexpr 的 static 数据成员会稍微好一些，允许我们对 float 类型以及其它字面值类型进行
            //    类内初始化
            constexpr static AccT const zero = 0.0f;
            //inline static AccT const zero = 0.0f;
        };
    
        template<typename T>
        auto accum (T const* beg, T const* end)
        {
            // return type is traits of the element type
            using AccT = typename AccumulationTraits<T>::AccT;
            AccT total = AccumulationTraits<T>::zero;                   // init total by trait value
            while (beg != end) {
                total += *beg;
                ++beg;
            }
            return total;
        }
    }
    
    /*
    但是无论是 const 还是 constexpr 都禁止对非字面值类型进行这一类初始化。比如，一个用
    户定义的任意精度的 BigInt 类型，可能就不是字面值类型，因为它可能会需要将一部分信息
    存储在堆上（这会阻碍其成为一个字面值类型），或者是因为我们所需要的构造函数不是
    constexpr 的。
     */
    
    /*
    namespace case4 {
        
        template <typename T>
        struct AccumulationTraits;
        
        class BigInt {
        public:
            string val;
            BigInt(string&& _val) : val(std::move(_val)) { cout << "BigInt(string&&)" << endl; }
        };
        
        class BigInt2 {
        public:
            string val;
            BigInt2(string&& _val) : val(std::move(_val)) { cout << "BigInt2(string&&)" << endl; }
        };
        
        template<>
        struct AccumulationTraits<BigInt> {
            using AccT = BigInt;
            // static constexpr BigInt zero = BigInt{""}; // ERROR: not a literal type 构造函数不是 constexpr 的
            // method 1
            static BigInt const zero;                       // declaration only
        };
        
        BigInt const AccumulationTraits<BigInt>::zero = BigInt{ "hello" };
        
        template<>
        struct AccumulationTraits<BigInt2> {
            using AccT = BigInt2;
            // static constexpr BigInt zero = BigInt{""}; // ERROR: not a literal type 构造函数不是 constexpr 的
            // method 2
            inline static BigInt2 const zero = BigInt2{ "world" };
        };
    
    }
     */
    
    /*
参数化的萃取
    在前面几节中，在 accum()里使用的萃取被称为固定的（fixed），这是因为一旦定义了解耦
    合萃取，在算法中它就不可以被替换。但是在某些情况下，这一类重写（overriding）行为
    却又是我们所期望的。比如，我们可能碰巧知道某一组 float 数值的和可以被安全地存储在
    一个 float 变量中，而这样做可能又会带来一些性能的提升。
    为了解决这一问题，可以为萃取引入一个新的模板参数 AT，其默认值由萃取模板决定：
        #ifndef ACCUM_HPP
        #define ACCUM_HPP
        #include "accumtraits4.hpp"
        
        template<typename T, typename AT = AccumulationTraits<T>>
        auto accum (T const* beg, T const* end)
        {
            typename AT::AccT total = AT::zero();
            while (beg != end) {
                total += *beg;
                ++beg;
            }
            return total;
        }
        #endif //ACCUM_HPP
    采用这种方式，一部分用户可以忽略掉额外模板参数，而对于那些有着特殊需求的用户，他
    们可以指定一个新的类型来取代默认类型。但是可以推断，大部分的模板用户永远都不需要
    显式的提供第二个模板参数，因为我们可以为第一个模板参数的每一种（通过推断得到的）
    类型都配置一个合适的默认值。
     */
    
    namespace case5 {
        template <typename T>
        struct AccumulationTraits;
        
        template <>
        struct AccumulationTraits<int> {
            using AccT = long long;
            static AccT const zero() {
                return 0;
            }
        };
        
        template<typename T, typename AT = AccumulationTraits<T>>
        auto accum (T const* beg, T const* end)
        {
            typename AT::AccT total = AT::zero();
            while (beg != end) {
                total += *beg;
                ++beg;
            }
            return total;
        }
        
        void test_accum() {
            int num[] = {1, 2, 3, 4, 5};
            cout << accum(num, num + 5) << endl;
        }
    
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_An_Example_Accumulating_a_Sequence_19_1()
{
    ch19_1::case2::test_accum();

    return 0;
}
