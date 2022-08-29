//
// Created by yangfeng on 2022/8/5.
//

#include "../headers.h"

/*
19.6 探测成员（Detecting Members）
  另一种对基于 SFINAE 的萃取的应用是，创建一个可以判断一个给定类型 T 是否含有名为 X
  的成员（类型或者非类型成员）的萃取。
  19.6.1 探测类型成员（Detecting Member Types）
  首先定义一个可以判断给定类型 T 是否含有类型成员 size_type 的萃取：
  #include <type_traits>
  // defines true_type and false_type
  // helper to ignore any number of template parameters:
  template<typename ...> using VoidT = void;
  五车书馆
  216
  // primary template:
  template<typename, typename = VoidT<>>
  struct HasSizeTypeT : std::false_type
  {};
  // partial specialization (may be SFINAE’d away):
  template<typename T>
  struct HasSizeTypeT<T, VoidT<typename T::size_type>> : std::true_type
  {} ;
  这里用到了在第 19.4.2 节介绍的剔除偏特化的方法。
  和往常已有，对于预测萃取，我们让一般情况派生自 std::false_type，因为某人情况下一个
  类型是没有 size_type 成员的。
  在这种情况下，我们只需要一个条件：
  typename T::size_type
  该条件只有在 T 含有类型成员 size_type 的时候才有效，这也正是我们所想要做的。如果对
  于某个类型 T，该条件无效，那么 SFINAE 会使偏特化实现被丢弃，我们就退回到主模板的
  情况。否则，偏特化有效并且会被有限选取。
  可以像下面这样使用萃取：
  std::cout << HasSizeTypeT<int>::value; // false
  struct CX {
  using size_type = std::size_t;
  };
  std::cout << HasSizeType<CX>::value; // true
  需要注意的是，如果类型成员 size_type 是 private 的，HasSizeTypeT 会返回 false，因为我们
  的萃取模板并没有访问该类型的特殊权限，因此 typename T::size_type 是无效的（触发
  SFINAE）。也就是说，该萃取所做的事情是测试我们是否能够访问类型成员 size_type。
  处理引用类型
  作为编程人员，应该已经很熟悉我们所考虑的主要问题之外的边边角角的问题了。诸如
  HasSizeTypeT 一类的萃取，在处理引用类型的时候可能会遇到让人意外的事情。比如，虽然
  如下的代码可以正常工作：
  struct CXR {
  using size_type = char&; // Note: type size_type is a reference type
  };
  五车书馆
  217
  std::cout << HasSizeTypeT<CXR>::value; // OK: prints true
  但是与之类似的代码却不会输出我们所期望的结果：
  std::cout << HasSizeTypeT<CXR&>::value; // OOPS: prints false
  std::cout << HasSizeTypeT<CXR&&>::value; // OOPS: prints false
  这或许会让人感到意外。引用类型确实没有成员，但是当我们使用引用的时候，结果表达式
  的类型是引用所指向的类型，因此我们可能会希望，当我们传递进来的模板参数是引用类型
  的时候，依然根据其指向的类型做判断。为了这一目的，可以在 HasSizeTypeT 的偏特化中使
  用我们之前介绍的 RemoveReference 萃取：
  template<typename T>
  struct HasSizeTypeT<T, VoidT<RemoveReference<T>::size_type>> :
  std::true_type {
  };
  注入类的名字（Injected Class Names）
  同样值得注意的是，对于注入类的名字（参见第 13.2.3 节），我们上述检测类型成员的萃取
  也会返回 true。比如对于：
  struct size_type {
  };
  struct Sizeable : size_type {
  };
  static_assert(HasSizeTypeT<Sizeable>::value, "Compiler bug: Injected
  class name missing");
  后面的 static_assert 会成功，因为 size_type 会将其自身的名字当作类型成员，而且这一成员
  会被继承。如果 static_assert 不会成功的话，那么我就发现了一个编译器的问题。
  19.6.2 探测任意类型成员
  在定义了诸如 HasSizeTypeT 的萃取之后，我们会很自然的想到该如何将该萃取参数化，以对
  任意名称的类型成员做探测。
  不幸的是，目前这一功能只能通过宏来实现，因为还没有语言机制可以被用来描述“潜在”
  的名字。当前不使用宏的、与该功能最接近的方法是使用泛型 lambda，正如在第 19.6.4 节
  介绍的那样。
  五车书馆
  218
  如下的宏可以满足我们的需求：
  #include <type_traits> // for true_type, false_type, and void_t
  #define
  DEFINE_HAS_TYPE(MemType) \
  template<typename, typename = std::void_t<>> \
  struct HasTypeT_##MemType \
  : std::false_type {
  }; \
  template<typename T> \
  struct HasTypeT_##MemType<T, std::void_t<typename T::MemType>> \
  : std::true_type { } // ; intentionally skipped
  每 一 次 对 DEFINE_HAS_TYPE(MemberType) 的 使 用 都 相 当 于 定 义 了 一 个 新 的
  HasTypeT_MemberType 萃取。比如，我们可以用之来探测一个类型是否有 value_type 或者
  char_type 类型成员：
  #include "hastype.hpp"
  #include <iostream>
  #include <vector>
  DEFINE_HAS_TYPE(value_type);
  DEFINE_HAS_TYPE(char_type);
  int main()
  {
  std::cout << "int::value_type: " << HasTypeT_value_type<int>::value
  << endl;
  std::cout << "std::vector<int>::value_type: " <<
  HasTypeT_value_type<std::vector<int>>::value << endl;
  std::cout << "std::iostream::value_type: " <<
  HasTypeT_value_type<std::iostream>::value << endl;
  std::cout << "std::iostream::char_type: " <<
  HasTypeT_char_type<std::iostream>::value << endl;
  }
  19.6.3 探测非类型成员
  可以继续修改上述萃取，以让其能够测试数据成员和（单个的）成员函数：
  #include <type_traits> // for true_type, false_type, and void_t
  #define
  DEFINE_HAS_MEMBER(Member) \
  template<typename, typename = std::void_t<>> \
  struct HasMemberT_##Member \
  : std::false_type { }; \
  五车书馆
  219
  template<typename T> \
  struct HasMemberT_##Member<T,
  std::void_t<decltype(&T::Member)>> \
  : std::true_type { } // ; intentionally skipped
  当&::Member 无效的时候，偏特化实现会被 SFINAE 掉。为了使条件有效，必须满足如下条
  件：
  
  Member 必须能够被用来没有歧义的识别出 T 的一个成员（比如，它不能是重载成员你
  函数的名字，也不能是多重继承中名字相同的成员的名字）。
  
  成员必须可以被访问。
  
  成员必须是非类型成员以及非枚举成员（否则前面的&会无效）。
  
  如果 T::Member 是 static 的数 据成员 ，那么与 其对应 的类型 必须没 有提供使 得
  &T::Member 无效的 operator&（比如，将 operator&设成不可访问的）。
  所有以上条件都满足之后，我们可以像下面这样使用该模板：
  #include "hasmember.hpp"
  #include <iostream>
  #include <vector>
  #include <utility>
  DEFINE_HAS_MEMBER(size);
  DEFINE_HAS_MEMBER(first);
  int main()
  {
  std::cout << "int::size: " << HasMemberT_size<int>::value << endl;
  std::cout << "std::vector<int>::size: " <<
  HasMemberT_size<std::vector<int>>::value << endl;
  std::cout << "std::pair<int,int>::first: " <<
  HasMemberT_first<std::pair<int,int>>::value << endl;
  }
  修改上面的偏特化实现以排除那些&T::Member 不是成员指针的情况（比如排除 static 数据
  成员的情况）并不会很难。类似地，也可以限制该偏特化仅适用于数据成员或者成员函数。
  探测成员函数
  注意，HasMember 萃取只可以被用来测试是否存在“唯一”一个与特定名称对应的成员。
  如果存在两个同名的成员的话，该测试也会失败，比如当我们测试某些重载成员函数是否存
  在的时候：
  DEFINE_HAS_MEMBER(begin);
  std::cout << HasMemberT_begin<std::vector<int>>::value; // false
  五车书馆
  220
  但是，正如在第 8.4.1 节所说的那样，SFINAE 会确保我们不会在函数模板声明中创建非法的
  类型和表达式，从而我们可以使用重载技术进一步测试某个表达式是否是病态的。
  也就是说，可以很简单地测试我们能否按照某种形式调用我们所感兴趣的函数，即使该函数
  被重载了，相关调用可以成功。正如在第 19.5 节介绍的 IsConvertibleT 一样，此处的关键是
  能否构造一个表达式，以测试我们能否在 decltype 中调用 begin()，并将该表达式用作额外
  的模板参数的默认值：
  #include <utility> // for declval
  #include <type_traits> // for true_type, false_type, and void_t
  // primary template:
  template<typename, typename = std::void_t<>>
  struct HasBeginT : std::false_type {
  };
  // partial specialization (may be SFINAE’d away):
  template<typename T>
  struct HasBeginT<T, std::void_t<decltype(std::declval<T>
  ().begin())>> : std::true_type {
  };
  这里我们使用 decltype(std::declval<T> ().begin())来测试是否能够调用 T 的
  begin()。
  探测其它的表达式
  相同的技术还可以被用于其它的表达式，甚至是多个表达式的组合。比如，我们可以测试对
  类型为 T1 和 T2 的对象，是否有合适的<运算符可用：
  #include <utility> // for declval
  #include <type_traits> // for true_type, false_type, and void_t
  // primary template:
  template<typename, typename, typename = std::void_t<>>
  struct HasLessT : std::false_type
  {};
  // partial specialization (may be SFINAE’d away):
  template<typename T1, typename T2>
  struct HasLessT<T1, T2, std::void_t<decltype(std::declval<T1>() <
  std::declval<T2>())>>: std::true_type
  {};
  和往常一样，问题的难点在于该如果为所要测试的条件定义一个有效的表达式，并通过使用
  decltype 将其放入 SFINAE 的上下文中，在该表达式无效的时候，SFINAE 机制会让我们最终
  五车书馆
  221
  选择主模板：
  decltype(std::declval<T1>() < std::declval<T2>())
  采用这种方式探测表达式有效性的萃取是很稳健的：如果表达式没有问题，它会返回 true，
  而如果<运算符有歧义，被删除，或者不可访问的话，它也可以准确的返回 false。
  我们可以像下面这样使用萃取：
  HasLessT<int, char>::value //yields true
  HasLessT<std::string, std::string>::value //yields true
  HasLessT<std::string, int>::value //yields false
  HasLessT<std::string, char*>::value //yields
  trueHasLessT<std::complex<double>,
  std::complex<double>>::value //yields false
  正如在第 2.3.1 节介绍的那样，我们也可以通过使用该萃取去要求模板参数 T 必须要支持<
  运算符：
  template<typename T>
  class C
  {
  static_assert(HasLessT<T>::value, "Class C requires comparable
  elements");
  ...
  };
  值得注意的是，基于 std::void_t 的特性，我们可以将多个限制条件放在同一个萃取中：
  #include <utility> // for declval
  #include <type_traits> // for true_type, false_type, and void_t
  // primary template:
  template<typename, typename = std::void_t<>>
  struct HasVariousT : std::false_type
  {};
  // partial specialization (may be SFINAE’d away):
  template<typename T>
  struct HasVariousT<T, std::void_t<decltype(
  std::declval<T> ().begin()),
  typename T::difference_type,
  typename T::iterator>> :
  std::true_type
  {};
  能够测试某一语法特性有效性的萃取是很有用的，基于有或者没有某一特定操作，可以用该
  萃取去客制化模板的行为。这一类萃取既可以被用于 SFINAE 友好的萃取（第 19.4.4 节）的
  一部分，也可以为基于类型特性的重载（第 20 章）提供帮助。
  五车书馆
  222
  19.6.4 用泛型 Lambda 探测成员
  在第 19.4.3 节介绍的 isValid lambda，提供了一种定义可以被用来测试成员的更为紧凑的技
  术，其主要的好处是在处理名称任意的成员时，不需要使用宏。
  下面这个例子展示了定义可以检测数据或者类型成员是否存在（比如 first 或者 size_type），
  或者有没有为两个不同类型的对象定义 operator <的萃取的方式：
  #include "isvalid.hpp"
  #include<iostream>
  #include<string>
  #include<utility>
  int main()
  {
  using namespace std;
  cout << boolalpha;
  // define to check for data member first:
  constexpr auto hasFirst = isValid([](auto x) ->
  decltype((void)valueT(x).first) {});
  cout << "hasFirst: " << hasFirst(type<pair<int,int>>) << endl; //
  true
  // define to check for member type size_type:
  constexpr auto hasSizeType = isValid([](auto x) -> typename
  decltype(valueT(x))::size_type { });
  struct CX {
  using size_type = std::size_t;
  };
  cout << "hasSizeType: " << hasSizeType(type<CX>) << endl; // true
  if constexpr(!hasSizeType(type<int>)) {
  cout << "int has no size_type\n";
  ...
  }
  // define to check for <:
  constexpr auto hasLess = isValid([](auto x, auto y) ->
  decltype(valueT(x) < valueT(y)) {});
  cout << hasLess(42, type<char>) << endl; //yields true
  cout << hasLess(type<string>, type<string>) << endl; //yields true
  五车书馆
  223
  cout << hasLess(type<string>, type<int>) << endl; //yields false
  cout << hasLess(type<string>, "hello") << endl; //yields true
  }
  请再次注意，hasSizeType 通过使用 std::decay 将参数 x 中的引用删除了，因为我们不能访问
  引用中的类型成员。如果不这么做，该萃取（对于引用类型）会始终返回 false，从而导致
  第二个重载的 isValidImpl<>被使用。
  为了能够使用统一的泛型语法（将类型用于模板参数），我们可以继续定义额外的辅助工具。
  比如：
  #include "isvalid.hpp"
  #include<iostream>
  #include<string>
  #include<utility>
  constexpr auto hasFirst
  = isValid([](auto&& x) -> decltype((void)&x.first) {});
  template<typename T>
  using HasFirstT = decltype(hasFirst(std::declval<T>()));
  constexpr auto hasSizeType = isValid([](auto&& x) -> typename
  std::decay_t<decltype(x)>::size_type {});
  template<typename T>
  using HasSizeTypeT = decltype(hasSizeType(std::declval<T>()));
  constexpr auto hasLess = isValid([](auto&& x, auto&& y) -> decltype(x
  < y) { });
  template<typename T1, typename T2>
  using HasLessT = decltype(hasLess(std::declval<T1>(),
  std::declval<T2>()));
  int main()
  {
  using namespace std;
  cout << "first: " << HasFirstT<pair<int,int>>::value << endl;
  // true
  struct CX {
  using size_type = std::size_t;
  };
  cout << "size_type: " << HasSizeTypeT<CX>::value << endl; // true
  cout << "size_type: " << HasSizeTypeT<int>::value << endl; // false
  五车书馆
  224
  cout << HasLessT<int, char>::value << endl; // true
  cout << HasLessT<string, string>::value << endl; // true
  cout << HasLessT<string, int>::value << endl; // false
  cout << HasLessT<string, char*>::value << endl; // true
  }
  现在可以像下面这样使用 HasFirstT：
  HasFirstT<std::pair<int,int>>::value
  它会为一个包含两个 int 的 pair 调用 hasFirst，其行为和之前的讨论一致。
 */

using namespace std;

/*
探测成员（Detecting Members）
  另一种对基于 SFINAE 的萃取的应用是，创建一个可以判断一个给定类型 T 是否含有名为 X的成员（类型或者非类型成员）的萃取。
 */

namespace ch19_6 {
  
  /*
19.6.1 探测类型成员（Detecting Member Types）
  首先定义一个可以判断给定类型 T 是否含有类型成员 size_type 的萃取：
  #include <type_traits>
  // defines true_type and false_type
  // helper to ignore any number of template parameters:
  template<typename ...> using VoidT = void;
  五车书馆
  216
  // primary template:
  template<typename, typename = VoidT<>>
  struct HasSizeTypeT : std::false_type
  {};
  // partial specialization (may be SFINAE’d away):
  template<typename T>
  struct HasSizeTypeT<T, VoidT<typename T::size_type>> : std::true_type
  {} ;
  这里用到了在第 19.4.2 节介绍的剔除偏特化的方法。
  和往常已有，对于预测萃取，我们让一般情况派生自 std::false_type，因为某人情况下一个
  类型是没有 size_type 成员的。
  在这种情况下，我们只需要一个条件：
  typename T::size_type
  该条件只有在 T 含有类型成员 size_type 的时候才有效，这也正是我们所想要做的。如果对
  于某个类型 T，该条件无效，那么 SFINAE 会使偏特化实现被丢弃，我们就退回到主模板的
  情况。否则，偏特化有效并且会被有限选取。
  可以像下面这样使用萃取：
  std::cout << HasSizeTypeT<int>::value; // false
  struct CX {
  using size_type = std::size_t;
  };
  std::cout << HasSizeType<CX>::value; // true
  需要注意的是，如果类型成员 size_type 是 private 的，HasSizeTypeT 会返回 false，因为我们
  的萃取模板并没有访问该类型的特殊权限，因此 typename T::size_type 是无效的（触发
  SFINAE）。也就是说，该萃取所做的事情是测试我们是否能够访问类型成员 size_type。
  处理引用类型
  作为编程人员，应该已经很熟悉我们所考虑的主要问题之外的边边角角的问题了。诸如
  HasSizeTypeT 一类的萃取，在处理引用类型的时候可能会遇到让人意外的事情。比如，虽然
  如下的代码可以正常工作：
  struct CXR {
  using size_type = char&; // Note: type size_type is a reference type
  };
  五车书馆
  217
  std::cout << HasSizeTypeT<CXR>::value; // OK: prints true
  但是与之类似的代码却不会输出我们所期望的结果：
  std::cout << HasSizeTypeT<CXR&>::value; // OOPS: prints false
  std::cout << HasSizeTypeT<CXR&&>::value; // OOPS: prints false
  这或许会让人感到意外。引用类型确实没有成员，但是当我们使用引用的时候，结果表达式
  的类型是引用所指向的类型，因此我们可能会希望，当我们传递进来的模板参数是引用类型
  的时候，依然根据其指向的类型做判断。为了这一目的，可以在 HasSizeTypeT 的偏特化中使
  用我们之前介绍的 RemoveReference 萃取：
  template<typename T>
  struct HasSizeTypeT<T, VoidT<RemoveReference<T>::size_type>> :
  std::true_type {
  };
  注入类的名字（Injected Class Names）
  同样值得注意的是，对于注入类的名字（参见第 13.2.3 节），我们上述检测类型成员的萃取
  也会返回 true。比如对于：
  struct size_type {
  };
  struct Sizeable : size_type {
  };
  static_assert(HasSizeTypeT<Sizeable>::value, "Compiler bug: Injected
  class name missing");
  后面的 static_assert 会成功，因为 size_type 会将其自身的名字当作类型成员，而且这一成员
  会被继承。如果 static_assert 不会成功的话，那么我就发现了一个编译器的问题。
   */
  namespace case1 {
    
    // defines true_type and false_type
    // helper to ignore any number of template parameters:
    template<typename ...> using VoidT = void;
    
    // primary template:
    template<typename, typename = VoidT<>>
    struct HasSizeTypeT : std::false_type {};
    
    // partial specialization (may be SFINAE’d away):
    /*
    template<typename T>
    struct HasSizeTypeT<T, VoidT<typename T::size_type>> : std::true_type {};
     */
    template<typename T>
    struct HasSizeTypeT<T, VoidT<typename std::remove_reference_t<T>::size_type>> : std::true_type {};
    
    struct CX {
      using size_type = std::size_t;
    };
    
    struct size_type { };
    struct Sizeable : size_type { };
    
    void test() {
      
      /*
      需要注意的是，如果类型成员 size_type 是 private 的，HasSizeTypeT 会返回 false，因为我们
        的萃取模板并没有访问该类型的特殊权限，因此 typename T::size_type 是无效的（触发
        SFINAE）。也就是说，该萃取所做的事情是测试我们是否能够访问类型成员 size_type。
       */
      cout << boolalpha;
      cout << HasSizeTypeT<int>::value << endl;
      cout << HasSizeTypeT<CX>::value << endl;
      cout << HasSizeTypeT<CX&>::value << endl;
      /*
       *注入类的名字（Injected Class Names）
        同样值得注意的是，对于注入类的名字（参见第 13.2.3 节），我们上述检测类型成员的萃取
        也会返回 true。比如对于：
        struct size_type {
        };
        struct Sizeable : size_type {
        };
        static_assert(HasSizeTypeT<Sizeable>::value, "Compiler bug: Injected
        class name missing");
        后面的 static_assert 会成功，因为 size_type 会将其自身的名字当作类型成员，而且这一成员
        会被继承。如果 static_assert 不会成功的话，那么我就发现了一个编译器的问题。
       */
      cout << HasSizeTypeT<Sizeable>::value << endl;
      static_assert(HasSizeTypeT<Sizeable>::value, "Compiler bug: Injected class name missing");
    }
    
    
    
  }
  
  /*
 19.6.2 探测任意类型成员
    在定义了诸如 HasSizeTypeT 的萃取之后，我们会很自然的想到该如何将该萃取参数化，以对
    任意名称的类型成员做探测。
    不幸的是，目前这一功能只能通过宏来实现，因为还没有语言机制可以被用来描述“潜在”
    的名字。当前不使用宏的、与该功能最接近的方法是使用泛型 lambda，正如在第 19.6.4 节
    介绍的那样。
    五车书馆
    218
    如下的宏可以满足我们的需求：
    #include <type_traits> // for true_type, false_type, and void_t
    #define
    DEFINE_HAS_TYPE(MemType) \
    template<typename, typename = std::void_t<>> \
    struct HasTypeT_##MemType \
    : std::false_type {
    }; \
    template<typename T> \
    struct HasTypeT_##MemType<T, std::void_t<typename T::MemType>> \
    : std::true_type { } // ; intentionally skipped
    每 一 次 对 DEFINE_HAS_TYPE(MemberType) 的 使 用 都 相 当 于 定 义 了 一 个 新 的
    HasTypeT_MemberType 萃取。比如，我们可以用之来探测一个类型是否有 value_type 或者
    char_type 类型成员：
    #include "hastype.hpp"
    #include <iostream>
    #include <vector>
    DEFINE_HAS_TYPE(value_type);
    DEFINE_HAS_TYPE(char_type);
    int main()
    {
    std::cout << "int::value_type: " << HasTypeT_value_type<int>::value
    << endl;
    std::cout << "std::vector<int>::value_type: " <<
    HasTypeT_value_type<std::vector<int>>::value << endl;
    std::cout << "std::iostream::value_type: " <<
    HasTypeT_value_type<std::iostream>::value << endl;
    std::cout << "std::iostream::char_type: " <<
    HasTypeT_char_type<std::iostream>::value << endl;
    }
   */
  
  namespace case2 {
    #define DEFINE_HAS_TYPE(MemType) \
    template<typename, typename = std::void_t<>> \
    struct HasTypeT_##MemType : std::false_type { }; \
    template<typename T> \
    struct HasTypeT_##MemType<T, std::void_t<typename T::MemType>> : std::true_type { } // ; intentionally skipped
    
    DEFINE_HAS_TYPE(value_type);
    DEFINE_HAS_TYPE(char_type);
    
    void test() {
      cout << boolalpha;
      
      cout << HasTypeT_value_type<int>::value << endl;
      cout << HasTypeT_value_type<case1::CX>::value << endl;
      
      cout << HasTypeT_char_type<std::iostream>::value << endl;
      cout << HasTypeT_value_type<std::iostream>::value << endl;
      
    
    }
  }
  
  /*
  19.6.3 探测非类型成员
   */
  
  namespace case3 {
    
    #define DEFINE_HAS_MEMBER(Member) \
    template<typename, typename = std::void_t<>> \
    struct HasMemberT_##Member : std::false_type { }; \
    template<typename T> \
    struct HasMemberT_##Member<T, std::void_t<decltype(&T::Member)>> : std::true_type { }   // ; intentionally skipped
    
    DEFINE_HAS_MEMBER(size);
    DEFINE_HAS_MEMBER(first);
    DEFINE_HAS_MEMBER(begin);
    
    // primary template:
    template<typename, typename = std::void_t<>>
    struct HasBeginT : std::false_type { };
    
    // partial specialization (may be SFINAE’d away):
    template<typename T>
    struct HasBeginT<T, std::void_t<decltype(std::declval<T>().begin())>> : std::true_type { };
    
    // primary template:
    template<typename, typename, typename = std::void_t<>>
    struct HasLessT : std::false_type {};
    
    // partial specialization (may be SFINAE’d away):
    template<typename T1, typename T2>
    struct HasLessT<T1, T2, std::void_t<decltype(std::declval<T1>() < std::declval<T2>())>>: std::true_type {};
    
    //正如在第 2.3.1 节介绍的那样，我们也可以通过使用该萃取去要求模板参数 T 必须要支持 < 运算符：
    template<typename T>
    class C
    {
      static_assert(HasLessT<T, T>::value, "Class C requires comparable elements");
    };
    
    
    struct [[maybe_unused]] CTest { int a {0}; int *p {nullptr}; };
    
    // primary template:
    template<typename, typename = std::void_t<>>
    struct HasVariousT : std::false_type {};
    
    // partial specialization (may be SFINAE’d away):
    template<typename T>
    struct HasVariousT<T, std::void_t<
                    decltype(std::declval<T> ().begin()),
                    typename T::difference_type,
                    typename T::iterator
                    >
              > : std::true_type {};
    
    void test() {
      cout << boolalpha;
      std::cout << "\tint::size: " << HasMemberT_size<int>::value << endl;
      std::cout << "\tstd::vector<int>::size: " << HasMemberT_size<std::vector<int>>::value << endl;
      std::cout << "\tstd::pair<int,int>::first: " << HasMemberT_first<std::pair<int, int>>::value << endl;
      /*
       注意，HasMember 萃取只可以被用来测试是否存在“唯一”一个与特定名称对应的成员。
        如果存在两个同名的成员的话，该测试也会失败，比如当我们测试某些重载成员函数是否存
        在的时候：
        DEFINE_HAS_MEMBER(begin);
        std::cout << HasMemberT_begin<std::vector<int>>::value; // false
      */
      std::cout << "\tvector<int> has begin():" << HasMemberT_begin<std::vector<int>>::value << endl; // false
      
      /*
      但是，正如在第 8.4.1 节所说的那样，SFINAE 会确保我们不会在函数模板声明中创建非法的
        类型和表达式，从而我们可以使用重载技术进一步测试某个表达式是否是病态的。
        也就是说，可以很简单地测试我们能否按照某种形式调用我们所感兴趣的函数，即使该函数
        被重载了，相关调用可以成功。正如在第 19.5 节介绍的 IsConvertibleT 一样，此处的关键是
        能否构造一个表达式，以测试我们能否在 decltype 中调用 begin()，并将该表达式用作额外
        的模板参数的默认值：
        #include <utility> // for declval
        #include <type_traits> // for true_type, false_type, and void_t
        // primary template:
        template<typename, typename = std::void_t<>>
        struct HasBeginT : std::false_type {
        };
        // partial specialization (may be SFINAE’d away):
        template<typename T>
        struct HasBeginT<T, std::void_t<decltype(std::declval<T>
        ().begin())>> : std::true_type {
        };
        这里我们使用 decltype(std::declval<T> ().begin())来测试是否能够调用 T 的
        begin()。
       */
  
      cout << "\ndetect member function:" << endl;
      cout << "\tvector<int> HasBeginT: " << HasBeginT<vector<int>>::value << endl;
      
      /*
    相同的技术还可以被用于其它的表达式，甚至是多个表达式的组合。比如，我们可以测试对
      类型为 T1 和 T2 的对象，是否有合适的<运算符可用：
      #include <utility> // for declval
      #include <type_traits> // for true_type, false_type, and void_t
      // primary template:
      template<typename, typename, typename = std::void_t<>>
      struct HasLessT : std::false_type
      {};
      // partial specialization (may be SFINAE’d away):
      template<typename T1, typename T2>
      struct HasLessT<T1, T2, std::void_t<decltype(std::declval<T1>() <
      std::declval<T2>())>>: std::true_type
      {};
      和往常一样，问题的难点在于该如果为所要测试的条件定义一个有效的表达式，并通过使用
      decltype 将其放入 SFINAE 的上下文中，在该表达式无效的时候，SFINAE 机制会让我们最终
      五车书馆
      221
      选择主模板：
      decltype(std::declval<T1>() < std::declval<T2>())
      采用这种方式探测表达式有效性的萃取是很稳健的：如果表达式没有问题，它会返回 true，
      而如果<运算符有歧义，被删除，或者不可访问的话，它也可以准确的返回 false。
      我们可以像下面这样使用萃取：
      HasLessT<int, char>::value //yields true
      HasLessT<std::string, std::string>::value //yields true
      HasLessT<std::string, int>::value //yields false
      HasLessT<std::string, char*>::value //yields
      trueHasLessT<std::complex<double>,
      std::complex<double>>::value //yields false
      正如在第 2.3.1 节介绍的那样，我们也可以通过使用该萃取去要求模板参数 T 必须要支持<
      运算符：
      template<typename T>
      class C
      {
      static_assert(HasLessT<T>::value, "Class C requires comparable
      elements");
      ...
      };
      值得注意的是，基于 std::void_t 的特性，我们可以将多个限制条件放在同一个萃取中：
      #include <utility> // for declval
      #include <type_traits> // for true_type, false_type, and void_t
      // primary template:
      template<typename, typename = std::void_t<>>
      struct HasVariousT : std::false_type
      {};
      // partial specialization (may be SFINAE’d away):
      template<typename T>
      struct HasVariousT<T, std::void_t<decltype(
      std::declval<T> ().begin()),
      typename T::difference_type,
      typename T::iterator>> :
      std::true_type
      {};
      能够测试某一语法特性有效性的萃取是很有用的，基于有或者没有某一特定操作，可以用该
      萃取去客制化模板的行为。这一类萃取既可以被用于 SFINAE 友好的萃取（第 19.4.4 节）的
      一部分，也可以为基于类型特性的重载（第 20 章）提供帮助
       */
  
      cout << "\ndetect other expression:" << endl;
      cout << "\tHasLessT<int, double>::value:\t" << HasLessT<int, double>::value << endl;
      cout << "\tHasLessT<float, double>::value:\t" << HasLessT<float, double>::value << endl;
      cout << "\tHasLessT<const char *, double>::value:\t" << HasLessT<const char *, double>::value << endl;
      cout << "\tHasLessT<const char *, double>::value:\t" << HasLessT<const char *, double>::value << endl;
      cout << "\tHasLessT<char, double>::value:\t" << HasLessT<char, double>::value << endl;
      
      // cout << "\nRequires CTest comparable elements:" << endl;
      // C<CTest> c1;
      
      /*
     值得注意的是，基于 std::void_t 的特性，我们可以将多个限制条件放在同一个萃取中：
      #include <utility> // for declval
      #include <type_traits> // for true_type, false_type, and void_t
      // primary template:
      template<typename, typename = std::void_t<>>
      struct HasVariousT : std::false_type
      {};
      // partial specialization (may be SFINAE’d away):
      template<typename T>
      struct HasVariousT<T, std::void_t<decltype(
      std::declval<T> ().begin()),
      typename T::difference_type,
      typename T::iterator>> :
      std::true_type
      {};
      能够测试某一语法特性有效性的萃取是很有用的，基于有或者没有某一特定操作，可以用该
      萃取去客制化模板的行为。这一类萃取既可以被用于 SFINAE 友好的萃取（第 19.4.4 节）的
      一部分，也可以为基于类型特性的重载（第 20 章）提供帮助。
       */
      
      
    }
    
  }
  
  /*
19.6.4 用泛型 Lambda 探测成员
  在第 19.4.3 节介绍的 isValid lambda，提供了一种定义可以被用来测试成员的更为紧凑的技
  术，其主要的好处是在处理名称任意的成员时，不需要使用宏。
  下面这个例子展示了定义可以检测数据或者类型成员是否存在（比如 first 或者 size_type），
  或者有没有为两个不同类型的对象定义 operator <的萃取的方式：
  #include "isvalid.hpp"
  #include<iostream>
  #include<string>
  #include<utility>
  int main()
  {
  using namespace std;
  cout << boolalpha;
  // define to check for data member first:
  constexpr auto hasFirst = isValid([](auto x) ->
  decltype((void)valueT(x).first) {});
  cout << "hasFirst: " << hasFirst(type<pair<int,int>>) << endl; //
  true
  // define to check for member type size_type:
  constexpr auto hasSizeType = isValid([](auto x) -> typename
  decltype(valueT(x))::size_type { });
  struct CX {
  using size_type = std::size_t;
  };
  cout << "hasSizeType: " << hasSizeType(type<CX>) << endl; // true
  if constexpr(!hasSizeType(type<int>)) {
  cout << "int has no size_type\n";
  ...
  }
  // define to check for <:
  constexpr auto hasLess = isValid([](auto x, auto y) ->
  decltype(valueT(x) < valueT(y)) {});
  cout << hasLess(42, type<char>) << endl; //yields true
  cout << hasLess(type<string>, type<string>) << endl; //yields true
  五车书馆
  223
  cout << hasLess(type<string>, type<int>) << endl; //yields false
  cout << hasLess(type<string>, "hello") << endl; //yields true
  }
  请再次注意，hasSizeType 通过使用 std::decay 将参数 x 中的引用删除了，因为我们不能访问
  引用中的类型成员。如果不这么做，该萃取（对于引用类型）会始终返回 false，从而导致
  第二个重载的 isValidImpl<>被使用。
  为了能够使用统一的泛型语法（将类型用于模板参数），我们可以继续定义额外的辅助工具。
  比如：
  #include "isvalid.hpp"
  #include<iostream>
  #include<string>
  #include<utility>
  constexpr auto hasFirst
  = isValid([](auto&& x) -> decltype((void)&x.first) {});
  template<typename T>
  using HasFirstT = decltype(hasFirst(std::declval<T>()));
  constexpr auto hasSizeType = isValid([](auto&& x) -> typename
  std::decay_t<decltype(x)>::size_type {});
  template<typename T>
  using HasSizeTypeT = decltype(hasSizeType(std::declval<T>()));
  constexpr auto hasLess = isValid([](auto&& x, auto&& y) -> decltype(x
  < y) { });
  template<typename T1, typename T2>
  using HasLessT = decltype(hasLess(std::declval<T1>(),
  std::declval<T2>()));
  int main()
  {
  using namespace std;
  cout << "first: " << HasFirstT<pair<int,int>>::value << endl;
  // true
  struct CX {
  using size_type = std::size_t;
  };
  cout << "size_type: " << HasSizeTypeT<CX>::value << endl; // true
  cout << "size_type: " << HasSizeTypeT<int>::value << endl; // false
  五车书馆
  224
  cout << HasLessT<int, char>::value << endl; // true
  cout << HasLessT<string, string>::value << endl; // true
  cout << HasLessT<string, int>::value << endl; // false
  cout << HasLessT<string, char*>::value << endl; // true
  }
  现在可以像下面这样使用 HasFirstT：
  HasFirstT<std::pair<int,int>>::value
  它会为一个包含两个 int 的 pair 调用 hasFirst，其行为和之前的讨论一致。
   */
  
  namespace case4 {
    
    // helper: checking validity of f (args...) for F f and Args... args:
    template<typename F, typename... Args, typename = decltype(std::declval<F>() (std::declval<Args&&>()...))>
    std::true_type isValidImpl(void*);
    
    // fallback if helper SFINAE’d out:
    template<typename F, typename... Args>
    std::false_type isValidImpl(...);
    
    // define a lambda that takes a lambda f and returns whether calling f with args is valid
    /*
    先从 isValid 的定义开始：它是一个类型为 lambda 闭包的 constexpr 变量。声明中必须要使
      用一个占位类型（placeholder type，代码中的 auto），因为 C++没有办法直接表达一个闭包
      类型。在 C++17 之前，lambda 表达式不能出现在 const 表达式中，因此上述代码只有在 C++17
      中才有效。因为 isValid 是闭包类型的，因此它可以被调用，但是它被调用之后返回的依然
      是一个闭包类型，返回结果由内部的 lambda 表达式生成。
      isValid 是一个萃取工厂（traits factory）：它会为其参数生成萃取，并用生成的萃取对对象进行测试。
     */
    inline constexpr
    auto isValid = [](auto f) {
      return [](auto&&... args) {
            return decltype(isValidImpl<decltype(f), decltype(args) && ...>(nullptr)){};
          };
    };
    
    // helper template to represent a type as a value
    template<typename T>
    struct TypeT { using Type = T; };
    
    // helper to wrap a type as a value
    // 允许我们用一个值代表一个类型, 对于通过这种方式获得的数值 x，我们可以通过使用 decltype(valueT(x))得到其原始类型，这也正是上面被传递给 isValid 的 lambda 所做的事情。
    template<typename T>
    constexpr auto type = TypeT<T>{};
    
    // helper to unwrap a wrapped type in unevaluated contexts
    template<typename T>
    T valueT(TypeT<T>); // no definition needed
    
    void test() {
      // define to check for data member first:
      /* // 此处void是显式弃值, 这样就只检查表达式合法，合法的情况下表达式的类型总是void */
      constexpr auto hasFirst = isValid([](auto x) -> decltype((void)valueT(x).first) {});
      cout << "hasFirst: " << hasFirst(type<pair<int,int>>) << endl; // true
      
      // define to check for member type size_type:
      constexpr auto hasSizeType = isValid([](auto x) -> typename decltype(valueT(x))::size_type { });
      
      struct CX { using size_type = std::size_t; };
      cout << "hasSizeType: " << hasSizeType(type<CX>) << endl; // true
      
      if constexpr (!hasSizeType(type<int>)) {
        cout << "int has no size_type\n";
      }
      
      // define to check for <:
      constexpr auto hasLess = isValid([](auto x, auto y) -> decltype(valueT(x) < valueT(y)) {});
      
      cout << hasLess(42, type<char>) << endl;        //yields true
      cout << hasLess(type<string>, type<string>) << endl;  //yields true
      cout << hasLess(type<string>, type<int>) << endl;     //yields false
      cout << hasLess(type<string>, "hello") << endl;     //yields true
      
    }

    
  }
  
  namespace case5 {
    constexpr auto hasFirst = case4::isValid([](auto&& x) -> decltype((void)&x.first) {});
    
    template<typename T>
    using HasFirstT = decltype(hasFirst(std::declval<T>()));
    
    constexpr auto hasSizeType = case4::isValid([](auto&& x) -> typename std::decay_t<decltype(x)>::size_type {});
    
    template<typename T>
    using HasSizeTypeT = decltype(hasSizeType(std::declval<T>()));
    
    constexpr auto hasLess = case4::isValid([](auto&& x, auto&& y) -> decltype(x < y) { });
    
    template<typename T1, typename T2>
    using HasLessT = decltype(hasLess(std::declval<T1>(), std::declval<T2>()));
    void test()
    {
      using namespace std;
      cout << "first: " << HasFirstT<pair<int,int>>::value << endl;
      // true
      struct CX {
        using size_type = std::size_t;
      };
      cout << "size_type: " << HasSizeTypeT<CX>::value << endl; // true
      cout << "size_type: " << HasSizeTypeT<int>::value << endl; // false
      cout << HasLessT<int, char>::value << endl; // true
      cout << HasLessT<string, string>::value << endl; // true
      cout << HasLessT<string, int>::value << endl; // false
      cout << HasLessT<string, char*>::value << endl; // true
    }
  }

  class Tmp {
  public:
  
  };

}

int
//main()
main_Detecting_Members_19_6()
{
  ch19_6::case5::test();

  
  return 0;
}