//
// Created by yangfeng on 2022/8/4.
//
#include "../headers.h"

/*
19.5 IsConvertibleT
  细节很重要。因此基于 SIFINAE 萃取的常规方法在实际中会变得更加复杂。为了展示这一复
  杂性，我们将定义一个能够判断一种类型是否可以被转化成另外一种类型的萃取，比如当我
  们期望某个基类或者其某一个子类作为参数的时候。IsConvertibleT 就可以判断其第一个类型
  参数是否可以被转换成第二个类型参数：
  #include <type_traits> // for true_type and false_type
  #include <utility> // for declval
  template<typename FROM, typename TO>
  struct IsConvertibleHelper {
  private:
  // test() trying to call the helper aux(TO) for a FROM passed as F :
  static void aux(TO);
  template<typename F, typename T,
  typename = decltype(aux(std::declval<F>()))>
  static std::true_type test(void*);
  // test() fallback:
  template<typename, typename>
  static std::false_type test(…);
  public:
  using Type = decltype(test<FROM>(nullptr));
  };
  template<typename FROM, typename TO>
  struct IsConvertibleT : IsConvertibleHelper<FROM, TO>::Type {
  };
  template<typename FROM, typename TO>
  using IsConvertible = typename IsConvertibleT<FROM, TO>::Type;
  template<typename FROM, typename TO>
  constexpr bool isConvertible = IsConvertibleT<FROM, TO>::value;
  这里我们使用了在第 19.4.1 节介绍的函数重载的方法。也就是说，我们在一个辅助类中定义
  了两个名为 test()的返回值类型不同的重载函数，并为该辅助类声明了 Type 成员类型：
  template<…> static std::true_type test(void*);
  template<…> static std::false_type test(…);
  …
  using Type = decltype(test<FROM>(nullptr));
  …
  template<typename FROM, typename TO>
  struct IsConvertibleT : IsConvertibleHelper<FROM, TO>::Type {
  };
  五车书馆
  214
  和往常一样，第一个 test()只有在所需的检查成功的时候才会被匹配到，第二个 test()则是应
  急方案。因此问题的关键就是让第一个 test()只有在类型 FROM 可以被转换成 TO 的时候才有
  效。为了实现这一目的，我们再次给第一个 test()分配了一个 dummy（并且无名）的模板参
  数，并将其初始化成只有当转换又消失才有效的内容。该模板参数不可以被推断，我们也不
  会为之提供显式的模板参数。因此它会被替换，而且当替换失败之后，该 test()声明会被丢
  弃掉。
  请再次注意，下面这种声明是不可以的：
  static void aux(TO);
  template<typename = decltype(aux(std::declval<FROM>()))>
  static char test(void*);
  这样当成员函数模板被解析的时候，FROM 和 TO 都已经完全确定了，因此对一组不适合做
  相应转换的类型，在调用 test()之前就会立即触发错误。
  由于这一原因，我们引入了作为成员函数模板参数的 F：
  static void aux(TO);
  template<typename F, typename = decltype(aux(std::declval<F> ()))>
  static char test(void*);
  并在 value 的初始化中将 FROM 类型用作调用 test()时的显式模板参数：
  static constexpr bool value
  = isSame<decltype(test<FROM>(nullptr)), char>;
  请注意这里是如何在不调用任何构造函数的情况下，通过使用在第 19.3.4 节介绍的
  std::declval 生成一个类型的值的。如果这个值可以被转换成 TO，对 aux()的调用就是有效的，
  相应的 test()调用也就会被匹配到。否则，会触发 SFINAE 错误，导致应急 test()被调用。
  然后，我们就可以像下面这样使用该萃取了：
  IsConvertibleT<int, int>::value //yields true
  IsConvertibleT<int, std::string>::value //yields false
  IsConvertibleT<char const*, std::string>::value //yields true
  IsConvertibleT<std::string, char const*>::value //yields false
  处理特殊情况
  下面 3 种情况还不能被上面的 IsConvertibleT 正确处理：
  1.
  向数组类型的转换要始终返回 false，但是在上面的代码中，aux()声明中的类型为 TO 的
  参数会退化成指针类型，因此对于某些 FROM 类型，它会返回 true。
  2.
  向指针类型的转换也应该始终返回 false，但是和 1 中的情况一样，上述实现只会将它
  们当作退化后的类型。
  3.
  向（被 const/volatile 修饰）的 void 类型的转换需要返回 true。但是不幸的是，在 TO 是
  五车书馆
  215
  void 的时候，上述实现甚至不能被正确实例化，因为参数类型不能包含 void 类型（而
  且 aux()的定义也用到了这一参数）。
  对于这几种情况，我们需要对它们进行额外的偏特化。但是，为所有可能的与 const 以及
  volatile 的组合情况都分别进行偏特化是很不明智的。相反，我们为辅助类模板引入了一个
  额外的模板参数:
  template<typename FROM, typename TO, bool = IsVoidT<TO>::value ||
  IsArrayT<TO>::value || IsFunctionT<TO>::value>
  struct IsConvertibleHelper {
  using Type = std::integral_constant<bool, IsVoidT<TO>::value &&
  IsVoidT<FROM>::value>;
  };
  template<typename FROM, typename TO>
  struct IsConvertibleHelper<FROM,TO,false> {
  … //previous implementation of IsConvertibleHelper here
  };
  额外的 bool 型模板参数能够保证，对于上面的所有特殊情况，都会最终使用主辅助萃取（而
  不是偏特化版本）。如果我们试图将 FROM 转换为数组或者函数，或者 FROM 是 void 而 TO
  不是，都会得到 false_type 的结果，不过对于 FROM 和 TO 都是 false_type 的情况，它也会返
  回 false_type。其它所有的情况，都会使第三个模板参数为 false，从而选择偏特化版本的实
  现（对应于我们之前介绍的实现）。
  至于 IsArrayT 和 IsFunctionT 的实现，请分别参见第 19.8.2 节和第 19.8.3 节。
  C++标准库中也提供了与之对应的 std::is_convertible<>，具体请参见第 D.3.3 节。
 */

using namespace std;

namespace ch19_5 {
  

  namespace case1 {
    template<typename FROM, typename TO>
    struct IsConvertibleHelper {
    private:
      // test() trying to call the helper aux(TO) for a FROM passed as F :
      static void aux(TO);
      
      template<typename F, typename = decltype(aux(std::declval<F>()))>
      static std::true_type test(void*);
    
      // test() fallback:
      template<typename>
      static std::false_type test(...);
  
    public:
      using Type = decltype(test<FROM>(nullptr));
    };
   
    template<typename FROM, typename TO>
    struct IsConvertibleT : IsConvertibleHelper<FROM, TO>::Type { };
  
    template<typename FROM, typename TO>
    using IsConvertible = typename IsConvertibleT<FROM, TO>::Type;
  
    template<typename FROM, typename TO>
    constexpr bool isConvertible = IsConvertibleT<FROM, TO>::value;
    
    void test() {
      cout << std::boolalpha;
      cout << isConvertible<int, int> << endl;
      cout << isConvertible<int, std::string> << endl;
      cout << isConvertible<char const *, std::string> << endl;
      cout << isConvertible<std::string, char const *> << endl;
    }
  }
  
  /*
处理特殊情况
  下面 3 种情况还不能被上面的 IsConvertibleT 正确处理：
  1.
  向数组类型的转换要始终返回 false，但是在上面的代码中，aux()声明中的类型为 TO 的
  参数会退化成指针类型，因此对于某些 FROM 类型，它会返回 true。
  2.
  向指针类型的转换也应该始终返回 false，但是和 1 中的情况一样，上述实现只会将它
  们当作退化后的类型。
  3.
  向（被 const/volatile 修饰）的 void 类型的转换需要返回 true。但是不幸的是，在 TO 是
  五车书馆
  215
  void 的时候，上述实现甚至不能被正确实例化，因为参数类型不能包含 void 类型（而
  且 aux()的定义也用到了这一参数）。
  对于这几种情况，我们需要对它们进行额外的偏特化。但是，为所有可能的与 const 以及
  volatile 的组合情况都分别进行偏特化是很不明智的。相反，我们为辅助类模板引入了一个
  额外的模板参数:
  template<typename FROM, typename TO, bool = IsVoidT<TO>::value ||
  IsArrayT<TO>::value || IsFunctionT<TO>::value>
  struct IsConvertibleHelper {
  using Type = std::integral_constant<bool, IsVoidT<TO>::value &&
  IsVoidT<FROM>::value>;
  };
  template<typename FROM, typename TO>
  struct IsConvertibleHelper<FROM,TO,false> {
  … //previous implementation of IsConvertibleHelper here
  };
  额外的 bool 型模板参数能够保证，对于上面的所有特殊情况，都会最终使用主辅助萃取（而
  不是偏特化版本）。如果我们试图将 FROM 转换为数组或者函数，或者 FROM 是 void 而 TO
  不是，都会得到 false_type 的结果，不过对于 FROM 和 TO 都是 false_type 的情况，它也会返
  回 false_type。其它所有的情况，都会使第三个模板参数为 false，从而选择偏特化版本的实
  现（对应于我们之前介绍的实现）。
  至于 IsArrayT 和 IsFunctionT 的实现，请分别参见第 19.8.2 节和第 19.8.3 节。
  C++标准库中也提供了与之对应的 std::is_convertible<>，具体请参见第 D.3.3 节。
   */
  namespace case2 {
    template<typename FROM, typename TO, bool = std::is_void<TO>::value ||
                          std::is_array<TO>::value || std::is_function<TO>::value>
    struct IsConvertibleHelper {
      using Type = std::integral_constant<bool, std::is_void<TO>::value &&
                            std::is_void<FROM>::value>;
    };
    
    template<typename FROM, typename TO>
    struct IsConvertibleHelper<FROM, TO, false> {
    private:
      // test() trying to call the helper aux(TO) for a FROM passed as F :
      static void aux(TO);
    
      template<typename F, typename = decltype(aux(std::declval<F>()))>
      static std::true_type test(void*);
    
      // test() fallback:
      template<typename>
      static std::false_type test(...);
  
    public:
      using Type = decltype(test<FROM>(nullptr));
    };
  
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
//main()
main_IsConvertibleT_19_5()
{
  ch19_5::case1::test();

  return 0;
}
