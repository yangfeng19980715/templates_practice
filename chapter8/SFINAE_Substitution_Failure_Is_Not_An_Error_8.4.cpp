//
// Created by yangfeng on 2022/7/27.
//

#include "../headers.h"

/*
SFINAE (Substitution Failure Is Not An Error, 替换失败不是错误)
 
  在 C++中，重载函数以支持不同类型的参数是很常规的操作。
   当编译器遇到一个重载函数的调用时，它必须分别考虑每一个重载版本，
   以选择其中类型最匹配的那一个（更多相关细节请参见附录 C）。
  
  在一个函数调用的备选方案中包含函数模板时，编译器首先要决定应该将什么样的模板参数用于各种模板方案，
    然后用这些参数替换函数模板的参数列表以及返回类型，
    最后评估替换后的函数模板和这个调用的匹配情况（就像常规函数一样）。
    
  但是这一替换过程可能会遇到问题：替换产生的结果可能没有意义。
    不过这一类型的替换不会导致错误，C++语言规则要求忽略掉这一类型的替换结果。
    这一原理被称为 SFINAE（发音类似 sfee-nay），代表的是“substitution failure is not an error”。
  
  但是上面讲到的替换过程和实际的实例化过程不一样（参见 2.2 节）：
    即使对那些最终被证明不需要被实例化的模板也要进行替换（不然就无法知道到底需不需要实例化）。
     不过它只会替换直接出现在函数模板声明中的相关内容（不包含函数体）。
     
  考虑如下的例子：
    // number of elements in a raw array:
    template<typename T, unsigned N>
    std::size_t len (T(&)[N])
    {
      return N;
    }
    
    // number of elements for a type having size_type:
    template<typename T>
    typename T::size_type len (T const& t)
    {
      return t.size();
    }
    
  这里定义了两个接受一个泛型参数的函数模板 len()：
    1. 第一个函数模板的参数类型是 T (&)[N]，也就是说它是一个包含了 N 个 T 型元素的数组。
    2. 第二个函数模板的参数类型就是简单的 T，除了返回类型要是 T::size_type 之外没有别的限制，
      这要求被传递的参数类型必须有一个 size_type 成员。
      
  当传递的参数是裸数组或者字符串常量时，只有那个为裸数组定义的函数模板能够匹配：
    int a[10];
    std::cout << len(a); // OK: only len() for array matches
    std::cout << len("tmp"); //OK: only len() for array matches
    
  如果只是从函数签名来看的话，对第二个函数模板也可以分别用 int[10]和 char const [4]替换类型参数 T，
    但是这种替换在处理返回类型 T::size_type 时会导致错误。
     因此对于这两个调用，第二个函数模板会被忽略掉。
     
  如果传递 std::vector<>作为参数的话，则只有第二个模板参数能够匹配：
    std::vector<int> v;
    std::cout << len(v); // OK: only len() for a type with size_type matches
    
  如果传递的是裸指针话，以上两个模板都不会被匹配上（但是不会因此而报错）。
  此时编译期会抱怨说没有发现合适的 len()函数：
    int* p;
    std::cout << len(p); // ERROR: no matching len() function found
    
  但是这和传递一个有 size_type 成员但是没有 size()成员函数的情况不一样。
    比如如果传递的参数是 std::allocator<>:
    std::allocator<int> x;
    std::cout << len(x); // ERROR: len() function found, but can’t size()
    
  此时编译器会匹配到第二个函数模板。
    因此不会报错说没有发现合适的 len()函数，而是会报一个编译期错误说对 std::allocator<int>而言 size()是一个无效调用。
    此时第二个模板函数不会被忽略掉。
    
  如果忽略掉那些在替换之后返回值类型为无效的备选项，那么编译器会选择另外一个参数类型匹配相差的备选项。
    比如：
    // number of elements in a raw array:
    template<typename T, unsigned N>
    std::size_t len (T(&)[N])
    {
      return N;
    }
    
    // number of elements for a type having size_type:
    template<typename T>
    typename T::size_type len (T const& t)
    {
      return t.size();
    }
    
  // 对所有类型的应急选项:
    std::size_t len (…)
    {
      return 0;
    }
    
  此处额外提供了一个通用函数 len()，它总会匹配所有的调用，
    但是其匹配情况也总是所有重载选项中最差的（通过省略号...匹配）（参见 C.2）。
    
  此时对于裸数组和 vector，都有两个函数可以匹配上，但是其中不是通过省略号（...）匹配的那一个是最佳匹配。
    对于指针，只有应急选项能够匹配上，此时编译器不会再报缺少适用于本次调用的 len()。
    
  不过对于 std::allocator<int>的调用，虽然第二个和第三个函数都能匹配上，但是第二个函数依然是最佳匹配项。
    因此编译器依然会报错说缺少 size()成员函数：
    int a[10];
    std::cout << len(a);        // OK: len() for array is best match
    std::cout << len("tmp");      //OK: len() for array is best match
    std::vector<int> v;
    std::cout << len(v);        // OK: len() for a type with size_type is best match
    int* p;
    std::cout << len(p);        // OK: only fallback len() matches
    std::allocator<int> x;
    std::cout << len(x);        // ERROR: 2nd len() function matches best, but can’t call size() for x
   
  请参见 15.7 节中更多关于 SFINAE 的内容，以及 19.4 节中一些 SFINAE 的应用示例。
*/

using namespace std;

namespace ch8_4 {
  /*
  SFINAE and Overload Resolution
    随着时间的推移，SFINAE 原理在模板开发者中变得越来越重要、越来越流行，以至于这个缩写常常被当作一个动词使用。
    当我们说“我们 SFINAE 掉了一个函数”时，意思是我们通过让模板在一些限制条件下产生无效代码，从而确保在这些条件下会忽略掉该模板。
    
    当你在C++标准里读到“除非在某些情况下，该模板不应该参与重载解析过程”时，
      它的意思就是“在该情况下，使用 SFINAE 方法 SFINAE 掉了这个函数模板”。
      
    比如 std::thread 类模板声明了如下构造函数：
      namespace std {
        class thread {
        public:
          …
          template<typename F, typename… Args>
          explicit thread(F&& f, Args&&… args);
          …
        };
      }
      
    并做了如下备注：
    备注：如果 decay_t<F>的类型和 std:thread 相同的话，该构造函数不应该参与重载解析过程。
    
    它的意思是如果在调用该构造函数模板时，使用 std::thread 作为第一个也是唯一一个参数的话，
      那么这个构造函数模板就会被忽略掉。
      
    这是因为一个类似的成员函数模板在某些情况下可能比预定义的 copy 或者
      move 构造函数更能匹配相关调用（相关细节请参见 6.2 节以及16.2.4 节）。
      
    通过 SFINAE 掉将该构造函数模板用于 thread 的情况，
      就可以确保在用一个 thread构造另一个 thread 的时候总是会调用预定义的 copy 或者 move 构造函数。
      
    但是使用该技术逐项禁用相关模板是不明智的。
      幸运的是标准库提供了更简单的禁用模板的方法。
      
    其中最广为人知的一个就是在 6.3 节介绍的 std::enable_if<>。
    因此典型的 std::thread 的实现如下：
    namespace std {
      class thread {
      public:
        template<typename F, typename… Args,
          typename =
          std::enable_if_t<!std::is_same_v<std::decay_t<F>, thread>>>
        explicit thread(F&& f, Args&&… args);
      };
    }
    关于 std:enable_if<>的实现请参见 20.3 节，它使用了部分特例化以及 SFINAE。
  */
  
  namespace case1 {
  
  }
  /*
通过 decltype 进行 SFINAE（此处是动词）的表达式
  对于有些限制条件，并不总是很容易地就能找到并设计出合适的表达式来 SFINAE 掉函数模板。
  
  比如，对于有 size_type 成员但是没有 size()成员函数的参数类型，
   我们想要保证会忽略掉函数模板 len()。
   如果没有在函数声明中以某种方式要求 size()成员函数必须存在，
   这个函数模板就会被选择并在实例化过程中导致错误：
   
    template<typename T>
    typename T::size_type len (T const& t)
    {
      return t.size();
    }
    std::allocator<int> x;
    std::cout << len(x) << ’\n’; //ERROR: len() selected, but x has no size()
    
  处理这一情况有一种常用模式或者说习惯用法：
   通过尾置返回类型语法（trailing return type syntax）来指定返回类型（在函数名前使用
    auto，并在函数名后面的->后指定返回类型）。
    
   通过 decltype 和逗号运算符定义返回类型。
  
   将所有需要成立的表达式放在逗号运算符的前面（为了预防可能会发生的运算符被重载
    的情况，需要将这些表达式的类型转换为 void）。
    
   在逗号运算符的末尾定义一个类型为返回类型的对象。
  
  比如：
    template<typename T>
    auto len (T const& t) -> decltype( (void)(t.size()),
    T::size_type() )
    {
      return t.size();
    }
  这里返回类型被定义成：
    decltype( (void)(t.size()), T::size_type() )
    
  类型指示符 decltype 的操作数是一组用逗号隔开的表达式，
     因此最后一个表达式 T::size_type()会产生一个类型为返回类型的对象（decltype 会将其转换为返回类型）。
     
  而在最后一个逗号前面的所有表达式都必须成立，在这个例子中逗号前面只有 t.size()。
   之所以将其类型转换为void，是为了避免因为用户重载了该表达式对应类型的逗号运算符而导致的不确定性。
   
  注意 decltype 的操作数是不会被计算的，也就是说可以不调用构造函数而直接创建其“dummy”对象，
   
   相关内容将在 11.2.3 节讨论。
  */
  
  namespace case2 {
  
  }
  
  
  namespace case3 {
  
  }
  
  namespace case4 {
  
  }

  class Tmp {
  public:
  
  };

}

int
//main()
main_SFINAE_substitution_failure_is_not_an_error()
{

  return 0;
}
