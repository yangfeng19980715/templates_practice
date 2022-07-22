//
// Created by yangfeng on 2022/7/22.
//
#include "../headers.h"

using namespace std;

/*
正如前几节介绍的那样，函数模板有多种传递参数的方式：
 
     将参数声明成按值传递：
        这一方法很简单，它会对字符串常量和裸数组的类型进行退化，但是对比较大的对象可能会受影响性能。
        在这种情况下，调用者仍然可以通过 std::cref()和 std::ref()按引用传递参数，但是要确保这一用法是有效的。
    
     将参数声明成按引用传递：
        对于比较大的对象这一方法能够提供比较好的性能。
        尤其是在下面几种情况下：
             将已经存在的对象（lvalue）按照左值引用传递。
             将临时对象（prvalue）或者被 std::move()转换为可移动的对象（xvalue）按右值引用传递。
             或者是将以上几种类型的对象按照转发引用传递。
            
    由于这几种情况下参数类型都不会退化，因此在传递字符串常量和裸数组时要格外小心。
    对于转发引用，需要意识到模板参数可能会被隐式推断为引用类型（引用折叠）。
*/

namespace ch7_6 {
    
    /*
一般性建议
    基于以上介绍，对于函数模板有如下建议：
    
    1. 默认情况下，将参数声明为按值传递。
        这样做比较简单，即使对字符串常量也可以正常工作。
        对于比较小的对象、临时对象以及可移动对象，其性能也还不错。
        对于比较大的对象，为了避免成本高昂的拷贝，可以使用 std::ref()和 std::cref()。
     
    2. 如果有充分的理由，也可以不这么做：
         如果需要一个参数用于输出，或者即用于输入也用于输出，那么就将这个参数按非const 引用传递。
            但是需要按照 7.2.2 节介绍的方法禁止其接受 const 对象。
            
         如果使用模板是为了转发它的参数，那么就使用完美转发（perfect forwarding）。
        也就是将参数声明为转发引用并在合适的地方使用 std::forward<>() 。
        考虑使用std::decay<>或者 std::common_type<>来处理不同的字符串常量类型以及裸数组类型的情况。
        
         如果重点考虑程序性能，而参数拷贝的成本又很高，那么就使用 const 引用。
        不过如果最终还是要对对象进行局部拷贝的话，这一条建议不适用。
     
    3. 如果你更了解程序的情况，可以不遵循这些建议。
         但是请不要仅凭直觉对性能做评估。在这方面即使是程序专家也会犯错。
         真正可靠的是：测试结果。
    */
    namespace case1 {
    
    }
    
    /*
不要过分泛型化
    值得注意的是，在实际应用中，函数模板通常并不是为了所有可能的类型定义的。
    而是有一定的限制。
     比如你可能已经知道函数模板的参数只会是某些类型的 vector。
    这时候最好不要将该函数模板定义的过于泛型化，否则，可能会有一些令人意外的副作用。
    针对这种情况应该使用如下的方式定义模板：
        template<typename T>
        void printVector (std::vector<T> const& v)
        {
        }
        
    这里通过的参数 v，可以确保 T 不会是引用类型，因为 vector 不能用引用作为其元素类型。
    而且将 vector 类型的参数声明为按值传递不会有什么好处，因为按值传递一个 vector 的成
    本明显会比较高昂（vector 的拷贝构造函数会拷贝 vector 中的所有元素）。
     此处如果直接将参数 v 的类型声明为 T，就不容易从函数模板的声明上看出该使用那种传递方式了。
    */
    
    namespace case2 {
    
    }
    
    
    /*
以 std::make_pair<>为例
    Std::make_pair<>()是一个很好的介绍参数传递机制相关陷阱的例子。
     使用它可以很方便的通过类型推断创建 std::pair<>对象。
     
    它的定义在各个版本的 C++中都不一样：
     在第一版 C++标准 C++98 中，std::make_pair<>被定义在 std 命名空间中，并且使用按引用传递来避免不必要的拷贝：
        template<typename T1, typename T2>
        pair<T1,T2> make_pair (T1 const& a, T2 const& b)
        {
            return pair<T1,T2>(a,b);
        }
    但是当使用 std::pair<>存储不同长度的字符串常量或者裸数组时，这样做会导致严重的问题。
    
     因此在 C++03 中，该函数模板被定义成按值传递参数：
        template<typename T1, typename T2>
        pair<T1,T2> make_pair (T1 a, T2 b)
        {
            return pair<T1,T2>(a,b);
        }
    正如你可以在 the rationale for the issue resolution 中读到的那样：
      看上去也这一方案对标准库的变化比其它两种建议都要小，而且其优点足以弥补它对性能造成的不利影响。
      
     不过在 C++11 中，由于 make_pair<>()需要支持移动语义，就必须使用转发引用。
         因此，其定义大体上是这样：
            template<typename T1, typename T2>
            constexpr pair<typename decay<T1>::type, typename decay<T2>::type>
            make_pair (T1&& a, T2&& b)
            {
                return pair<typename decay<T1>::type,
                            typename decay<T2>::type>(forward<T1>(a), forward<T2>(b));
            }
            
      完整的实现还要复杂的多：为了支持 std::ref() 和 std::cref()， 该函数会将std::reference_wrapper 展开成真正的引用。
        目前 C++标准库在很多地方都使用了类似的方法对参数进行完美转发，而且通常都会结合std::decay<>使用。
    */

    namespace case3 {
    
    }
    
    class Tmp {
    public:
    
    };

}

int
main()
{

    return 0;
}
