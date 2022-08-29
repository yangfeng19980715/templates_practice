//
// Created by yangfeng on 2022/7/18.
//
#include "../headers.h"

using namespace std;

/*
 *   从 C++17 开始，提供了一种可以用来计算参数包（可以有初始值）中所有参数运算结果的二元运算符。
 *
 *
 *
 */

namespace ch4_2 {
  /*
   *
     比如，下面的函数会返回 s 中所有参数的和：
     注意返回语句中的括号是折叠表达式的一部分，不能被省略。
     
      template<typename… T>
      auto foldSum (T… s) {
        return (… + s);  // ((s1 + s2) + s3) …
      }
      
    如果参数包是空的，这个表达式将是不合规范的（不过此时对于运算符&&，结果会是 true，
    对运算符||，结果会是 false，对于逗号运算符，结果会是 void()）。
   */
  class Fold1 {
  public:
    template <typename... T>
    auto foldSum(T ... s) {
      return (... + s);
    }
    
    void test_foldSum() {
      auto ret = foldSum(1, 2, 3, 4, 5);
      cout << ret << endl;
    }
    
  };
  
  /*
   *
  折叠表达式的出现让我们不必再用递归实例化模板的方式来处理参数包。在 C++17 之前，你必须这么实现：
    template<typename T>
    auto foldSumRec (T arg) {
      return arg;
    }
    
    template<typename T1, typename... Ts>
    auto foldSumRec (T1 arg1, Ts... otherArgs) {
      return arg1 + foldSumRec(otherArgs...);
    }
    
    这样的实现不仅写起来麻烦，对 C++ 编译器来说也很难处理。使用如下写法：
    template<typename... T>
    auto foldSum (T... args) {
      return (... + args);  // arg1 + arg2 + arg3...
    }
    能显著的减少程序员和编译器的工作量。
   */
  
  class Fold2 {
  public:
    template <typename T>
    auto foldSumRec(T arg) {
      return arg;
    }
    
    template <typename T1, typename... Ts>
    auto foldSumRec(T1 arg1, Ts... args) {
      return arg1 + foldSumRec(args...);
    }
    
    void test_fold2() {
      auto ret = foldSumRec(1, 2);
      cout << ret << endl;
    }
  };
  
  /*
   * 给定一个参数 args 和一个操作符 op，C++17 允许我们这么写：
    • 一元左折叠
      ( … op args )
      将会展开为：((arg1 op arg2) op arg3) op …
    • 一元右折叠
      ( args op … )
      将会展开为：arg1 op (arg2 op … (argN­1 op argN))
    括号是必须的，然而，括号和省略号 (…) 之间并不需要用空格分隔。
    左折叠和右折叠的不同比想象中更大。
    例如，当你使用 + 时可能会产生不同的效果。
    
    
    在任何情况下，从左向右求值都是符合直觉的。因此，更推荐使用左折叠的语法：
      (... + args);  // 推 荐 的 折 叠 表 达 式 语 法
   */
  
  
  /*
   *  当使用折叠表达式处理空参数包时，将遵循如下规则：
      • 如果使用了 && 运算符，值为 true。
      • 如果使用了 || 运算符，值为 false。
      • 如果使用了逗号运算符，值为 void()。
      • 使用所有其他的运算符，都会引发格式错误
   */
  
  /*
   * 对于所有其他的情况，你可以添加一个初始值：
      给定一个参数包 args，一个初始值 value，一个操作符 op，
    C++17 允许我们这么写：
      • 二元左折叠
        ( value op … op args )
        将会展开为：(((value op arg1) op arg2) op arg3) op …
      • 二元右折叠
        ( args op … op value )
        将会展开为：arg1 op (arg2 op … (argN op value))
        
      省略号两侧的 op 必须相同。
   */
  class Fold3 {
  public:
    template <typename ... T>
    auto foldSumLeft(T ... args) {
      return (0 + ... + args);  // 即使sizeof...(s) == 0也能正常工作
    }
    
    void test_fole_sum_left() {
      auto ret = foldSumLeft(1, 2, 3);
      cout << ret << endl;
    }
    
    
  };
  
  /*
   * 对于一元折叠表达式来说，不同的求值顺序比想象中的更重要。对于二元表达式来说，也更推荐左折叠的方式：
      (val + ... + args);  // 推 荐 的 二 元 折 叠 表 达 式 语 法
      
    有时候第一个操作数是特殊的，比如下面的例子：
    template<typename... T>
    void print (const T&... args)
    {
      (std::cout << ... << args) << '\n';
    }
    这里，传递给 print() 的第一个参数输出之后将返回输出流，
    所以后面的参数可以继续输出。其他的实现可能不能编译或者产生一些意料之外的结果。例如，
      std::cout << (args << ... << '\n');
    类似 print(1) 的调用可以编译，但会打印出 1 左移'\n' 位之后的值，'\n' 的值通常是 10，所以结果是 1024。
   */
  class Fold4 {
  public:
    template <typename ... T>
    void print(const T& ... args) {
      (std::cout << ... << args) << '\n';
    }
    
    void test_print() {
      print("hello", 13, "yes");
    }
  };
  
  class Fold5 {
  public:
    template <typename T>
    const T& spaceBefore(const T & arg) {
      std::cout << " ";
      return arg;
    }
    
    template <typename First, typename ... Types>
    void print(const First & first, const Types& ... args) {
      std::cout << first;
      (std::cout << ... << spaceBefore(args)) << '\n';
    }
    
    void test() {
      print("hello", 15, "world");
    }
  };
  
  class Fold6 {
  public:
    // 我们也可以使用 lambda 来在 print() 内定义 spaceBefore()：
    template<typename First, typename... Args>
    void print1(const First& firstarg, const Args&... args) {
      std::cout << firstarg;
      auto spaceBefore = [](const auto& arg) {
        std::cout << ' ';
        return arg;
      };
      (std::cout << ... << spaceBefore(args)) << '\n';
    }
    /*
    然而，注意默认情况下 lambda 以值返回对象，这意味着会创建参数的不必要的拷贝。解决方法是显式指明返回
        类型为 const auto& 或者 decltype(auto)：
    */
    template<typename First, typename... Args>
    void print2(const First& firstarg, const Args&... args) {
      std::cout << firstarg;
      auto spaceBefore = [](const auto& arg) -> const auto& {
        std::cout << ' ';
        return arg;
      };
      (std::cout << ... << spaceBefore(args)) << '\n';
    }
  
    template<typename First, typename... Args>
    void print3(const First& firstarg, const Args&... args) {
      std::cout << firstarg;
      (std::cout << ... << [] (const auto& arg) -> decltype(auto) {
          std::cout << ' ';
          return arg;
      }(args)) << '\n';
    }
  };
  
  class Fold7 {
  public:
    // 不过，一个更简单的实现 print() 的方法是使用一个 lambda 输出空格和参数，然后在一元折叠表达式里使用它：
    template<typename First, typename... Args>
    void print(First first, const Args&... args) {
      std::cout << first;
      auto outWithSpace = [] (const auto& arg) {
        std::cout << ' ' << arg;
      };
      (... , outWithSpace(args));  // 逗号表达式，折叠函数调用
      std::cout << '\n';
    }
    
    void test() {
      print(1, "hello", 2, "yes", 3.4);
    }
  };
  
  class Fold8 {
  public:
    /*
      折叠函数调用
        折叠表达式可以用于逗号运算符，这样就可以在一条语句里进行多次函数调用。
        也就是说，你现在可以简单写出如下实现：
        template<typename... Types>
        void callFoo(const Types&... args)
        {
          (... , foo(args));  // 调 用foo(arg1)，foo(arg2)，foo(arg3)，...
        }
        
        来对所有参数调用函数 foo()
     */
    template<typename T>
    void print(T t) {
      cout << t << '\t';
    }
    template<typename... Types>
    void callFoo(const Types & ... args) {
      (..., print(args));
    }
    
    void test() {
      callFoo(1, "hello", 2.3, std::string("yes"));
    }
  
  };
  
  class Fold9 {
  public:
    /*
    另外，如果需要支持移动语义：
    template<typename... Types>
    void callFoo(Types&&... args)
    {
      (... , foo(std::forward<Types>(args))); // 调 用foo(arg1)，foo(arg2)，...
    }
     */
    template <typename T>
    void foo(T && arg) {
      std::cout << arg;
    }
    template <typename ... Types>
    void callFoo(Types... args) {
      (..., foo(std::forward<Types>(args)));
    }
    
    /*
    如果 foo() 函数返回的类型重载了逗号运算符，那么代码行为可能会改变。
     为了保证这种情况下代码依然安全，你需要把返回值转换为 void：
    template<typename... Types>
    void callFoo(const Types&... args)
    {
      (... , (void)foo(std::forward<Types>(args))); // 调 用foo(arg1)，foo(arg2)，...
    }
     */
    template<typename... Types>
    void callFoo2(const Types&... args)
    {
      (... , (void)foo(std::forward<Types>(args))); // 调 用foo(arg1)，foo(arg2)，...
    }
  
    
    /*
    注意自然情况下，对于逗号运算符不管我们是左折叠还是右折叠都是一样的。
     函数调用们总是会从左向右执行。如下写法：
      (foo(args) , ...);
    中的括号只是把后边的调用括在一起，因此首先是第一个 foo() 调用，之后是被括起来的两个 foo() 调用：
      foo(arg1) , (foo(arg2) , foo(arg3));
    然而，因为逗号表达式的求值顺序通常是自左向右，
     所以第一个调用通常发生在括号里的两个调用之前，并且括号里左侧的调用在右侧的调用之前。
     
    不过，因为左折叠更符合自然的求值顺序，因此在使用折叠表达式进行多次函数调用时还是推荐使用左折叠。
    */
    
  };
  
  class Fold10 {
  public:
    /*
     * 组合哈希函数
      另一个使用逗号折叠表达式的例子是组合哈希函数。可以用如下的方法完成：
        template<typename T>
        void hashCombine (std::size_t& seed, const T& val)
        {
          seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        
        template<typename... Types>
        std::size_t combinedHashValue (const Types&... args)
        {
          std::size_t seed = 0;
          // 初 始 化seed
          (... , hashCombine(seed, args));
          // 链 式 调 用hashCombine()
          return seed;
        }
     */
  };
  
  class Fold11 {
  public:
    /*
    折叠基类的函数调用
    折叠表达式可以在更复杂的场景中使用。例如，你可以折叠逗号表达式来调用可变数量基类的成员函数：
     
      #include <iostream>
      // 可 变 数 量 基 类 的 模 板
      template<typename... Bases>
      class MultiBase : private Bases...
      {
      public:
        void print() {
        // 调 用 所 有 基 类 的print()函 数
        (... , Bases::print());
        }
      };
      
      struct A {
        void print() { std::cout << "A::print()\n"; }
      }
      struct B {
        void print() { std::cout << "B::print()\n"; }
      }
      struct C {
        void print() { std::cout << "C::print()\n"; }
      }
      int main()
      {
        MultiBase<A, B, C> mb;
        mb.print();
      }
     */
  };
  
  class Fold12 {
  public:
    /*
      使用折叠表达式处理类型
        通过使用类型特征，我们也可以使用折叠表达式来处理模板参数包（任意数量的模板类型参数）。
        例如，你可以使用折叠表达式来判断一些类型是否相同：
        
        #include <type_traits>
        
        // 检 查 是 否 所 有 类 型 都 相 同：
        template<typename T1, typename... TN>
        struct IsHomogeneous {
          static constexpr bool value = (std::is_same_v<T1, TN> && ...);
        };
        
        // 检 查 是 否 所 有 传 入 的 参 数 类 型 相 同：
        template<typename T1, typename... TN>
        constexpr bool isHomogeneous(T1, TN...)
        {
          return (std::is_same_v<T1, TN> && ...);
        }
        
        
        类型特征 IsHomogeneous<> 可以像下面这样使用：
          IsHomogeneous<int, MyType, decltype(42)>::value
        这种情况下，折叠表达式将会展开为：
          std::is_same_v<int, MyType> && std::is_same_v<int, decltype(42)>
        函数模板 isHomogeneous<>() 可以像下面这样使用：
          isHomogeneous(43, ‐1, "hello", nullptr)
        在这种情况下，折叠表达式将会展开为：
          std::is_same_v<int, int> && std::is_same_v<int, const char*> && is_same_v<int, std::nullptr_t>
          
        像通常一样，运算符 && 会短路求值（出现第一个 false 时就会停止运算）。
        标准库里std::array<> 的推导指引就使用了这个特性。
     */
  };
  
  
  
  

  class Tmp {
  public:
  
  };

}

int
//main()
main_fold_expressions()
{
  auto fold = ch4_2::Fold8{};
  fold.test();

  return 0;
}
