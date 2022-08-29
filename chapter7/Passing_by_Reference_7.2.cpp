//
// Created by yangfeng on 2022/7/22.
//
#include "../headers.h"

/*
按引用传递不会拷贝对象（因为形参将引用被传递的实参）。
 而且，按引用传递时参数类型也不会退化（decay）。
 不过，并不是在所有情况下都能使用按引用传递，
  即使在能使用的地方，有时候被推断出来的模板参数类型也会带来不少问题。
 
*/

using namespace std;

namespace ch7_2 {
  /*
  为了避免（不必要的）拷贝，在传递非临时对象作为参数时，可以使用 const 引用传递。
   比如：
    template<typename T>
    void printR (T const& arg) {
    }
    
  这个模板永远不会拷贝被传递对象（不管拷贝成本是高还是低）。
  
   即使是按引用传递一个 int 类型的变量，虽然这样可能会事与愿违（不会提高性能，见下段中的解释），也依然不会拷贝。
   因此如下调用：
    int i = 42;
    printR(i); // passes reference instead of just copying i
    
  会将 printR()实例化为：
    void printR(int const& arg) {
    }
    
  这样做之所以不能提高性能，是因为在底层实现上，按引用传递还是通过传递参数的地址实现的。
   地址会被简单编码，这样可以提高从调用者向被调用者传递地址的效率。
   
   不过按地址传递可能会使编译器在编译调用者的代码时有一些困惑：
     被调用者会怎么处理这个地址？理论上被调用者可以随意更改该地址指向的内容。
     这样编译器就要假设在这次调用之后，所有缓存在寄存器中的值可能都会变为无效。
     而重新载入这些变量的值可能会很耗时（可能比拷贝对象的成本高很多）。
   
   你或许会问在按 const 引用传递参数时：为什么编译器不能推断出被调用者不会改变参数的值？
   不幸的是，确实不能，因为调用者可能会通过它自己的非 const引用修改被引用对象的值（
     这个解释太好，另一种情况是被调用者可以通过 const_cast 移除参数中的 const）。
     
  不过对可以 inline 的函数，情况可能会好一些：
   如果编译器可以展开 inline 函数，那么它就可以基于调用者和被调用者的信息，推断出被传递地址中的值是否会被更改。
   
   函数模板通常总是很短，因此很可能会被做 inline 展开。
     但是如果模板中有复杂的算法逻辑，那么它大概率就不会被做 inline 展开了。
   */
  
  namespace case1 {
  
  }
  
  /*
  按引用传递不会做类型退化（decay）
   
    按引用传递参数时，其类型不会退化（decay）。
    也就是说不会把裸数组转换为指针，也不会移除 const 和 volatile 等限制符。
    而且由于调用参数被声明为 T const &，被推断出来的模板参数 T 的类型将不包含 const。
    
    比如：
      template<typename T>
      void printR (T const& arg) {
      }
      
      std::string const c = "hi";
      
      printR(c);    // T deduced as std::string, arg is std::string const&
      printR("hi");   // T deduced as char[3], arg is char const(&)[3]
      
      int arr[4];
      printR(arr);  // T deduced as int[4], arg is int const(&)[4]
      
    因此对于在 printR()中用 T 声明的变量，它们的类型中也不会包含 const。
  */
  namespace case2 {
    template <typename T>
    T printR(T const & arg) {
      T val = arg;
      return val;
    }
    
    void test_func() {
      std::string const c = "hi";
      auto ret = printR(c);
      
      
    }
  
    
  }
  
  /*
按非 const 引用传递
  如果想通过调用参数来返回变量值（比如修改被传递变量的值），就需要使用非 const 引用（要么就使用指针）。
   同样这时候也不会拷贝被传递的参数。被调用的函数模板可以直接访问被传递的参数。
   
  考虑如下情况：
    template<typename T>
    void outR (T& arg) {
    }
    
  注意对于 outR()，通常不允许将临时变量（prvalue）
     或者通过 std::move()处理过的已存在的变量（xvalue）用作其参数：
    std::string returnString();
    std::string s = "hi";
    
    outR(s);          //OK: T deduced as std::string, arg is std::string&
    outR(std::string("hi"));  //ERROR: not allowed to pass a temporary (prvalue)
    outR(returnString());     // ERROR: not allowed to pass a temporary (prvalue)
    outR(std::move(s));     // ERROR: not allowed to pass an xvalue
    
  同样可以传递非 const 类型的裸数组，其类型也不会 decay：
    int arr[4];
    outR(arr);          // OK: T deduced as int[4], arg is int(&)[4]
    
  这样就可以修改数组中元素的值，也可以处理数组的长度。比如：
    template<typename T>
    void outR (T& arg) {
      if (std::is_array<T>::value) {
        std::cout << "got array of " << std::extent<T>::value << "elems\n";
      }
    }
    
  但是在这里情况有一些复杂。
   此时如果传递的参数是 const 的，arg 的类型就有可能被推断为 const 引用，
     也就是说这时可以传递一个右值（rvalue）作为参数，但是模板所期望的参数类型却是左值（lvalue）：
     
    std::string const c = "hi";
    outR(c);            // OK: T deduced as std::string const
    outR(returnConstString());    // OK: same if returnConstString() returns const string
    outR(std::move(c));       // OK: T deduced as std::string const6
    outR("hi");           // OK: T deduced as char const[3]
    
  在这种情况下，在函数模板内部，任何试图更改被传递参数的值的行为都是错误的。
    在调用表达式中也可以传递一个 const 对象，
     但是当函数被充分实例化之后（可能发生在接接下来的编译过程中），
     任何试图更改参数值的行为都会触发错误（但是这有可能发生在被调用模板的很深层次逻辑中，具体细节请参见 9.4 节）。
     
  如果想禁止向非 const 应用传递 const 对象，有如下选择：
  
   使用 static_assert 触发一个编译期错误：
    template<typename T>
    void outR (T& arg) {
      static_assert(!std::is_const<T>::value, "out parameter of foo<T>(T&) is const");
    }
    
   通过使用 std::enable_if<>（参见 6.3 节）禁用该情况下的模板：
    template<typename T,
    typename = std::enable_if_t<!std::is_const<T>::value>
    void outR (T& arg) {
    }
    
  或者是在 concepts 被支持之后，通过 concepts 来禁用该模板（参见 6.5 节以及附录 E）：
    template<typename T>
    requires !std::is_const_v<T>
    void outR (T& arg) {
    }
  */
  
  namespace case3 {
    
    template <typename T>
    void outR(T & arg) {
      static_assert(!std::is_const<T>::value, "parameter of outR<T>(T&) is const");
      if (std::is_array<T>::value) {
        std::cout << "got array of " << std::extent<T>::value << " elements" << endl;
      }
    }
    
    std::string getString() { return "hello"; }
    const std::string getStringConst() { return "hello"; }
    
    void test_func() {
      std::string const c = "hi";
      /*
      outR(c);
      outR(getStringConst());
      outR(std::move(c));
      outR("hi");
       */
      
    }
  
    void test_func_bak() {
      std::string s{ "world" };
      // outR(s1);
      // outR(std::string{"hi"}); // error
      // outR(getString());  // error
      // outR(std::move(s));  // error
      
      int arr[4] {9, 8, 7, 6};
      outR(arr); // ok
  
    }
    
  
  }
  /*
按转发引用传递参数（Forwarding Reference）
  使用引用调用（call-by-reference）的一个原因是可以对参数进行完美转发（perfect forward）（参见 6.1 节）。
   但是请记住在使用转发引用时（forwarding reference，被定义成一个模板参数的右值引用（rvalue reference）），有它自己特殊的规则。
   
  考虑如下代码：
    template<typename T>
    void passR (T&& arg) { // arg declared as forwarding reference
    …
    }
    
  可以将任意类型的参数传递给转发引用，而且和往常的按引用传递一样，都不会创建被传递参数的备份：
    std::string s = "hi";
    passR(s);             // OK: T deduced as std::string& (also the type of arg)
    passR(std::string("hi"));     // OK: T deduced as std::string, arg is std::string&&
    passR(returnString());      // OK: T deduced as std::string, arg is std::string&&
    passR(std::move(s));      // OK: T deduced as std::string, arg is std::string&&
    passR(arr);           // OK: T deduced as int(&)[4] (also the type of arg)
    
  但是，这种情况下类型推断的特殊规则可能会导致意想不到的结果：
    std::string const c = "hi";
    passR(c);         //OK: T deduced as std::string const&
    passR("hi");      //OK: T deduced as char const(&)[3] (also the type of arg)
    int arr[4];
    passR(arr);       //OK: T deduced as int (&)[4] (also the type of arg)
    
  在以上三种情况中，
     都可以在 passR()内部从 arg 的类型得知被传递的参数是一个右值（rvalue）还是一个 const 或者非 const 的左值（lvalue）。
  这是唯一一种可以传递一个参数，并用它来区分以上三种情况的方法。
  看上去将一个参数声明为转发引用总是完美的。
  
  但是，没有免费的午餐。
  比如，由于转发引用是唯一一种可以将模板参数 T 隐式推断为引用的情况，
     此时如果在模板内部直接用 T 声明一个未初始化的局部变量，就会触发一个错误（引用对象在创建的时候必须被初始化）：
     
    template<typename T>
    void passR(T&& arg) {     // arg is a forwarding reference
      T x;          // for passed lvalues, x is a reference, which requires an initializer
    }
    foo(42);          // OK: T deduced as int
    int i;
    foo(i);           // ERROR: T deduced as int&, which makes the declaration of x in passR() invalid
   
  */
  
  namespace case4 {
    
    template <typename T>
    void parseR(T && arg) {     // arg is forwarding reference
      T x;  // for passed lvalues, x is a reference, which requires an initializer
    }
    
    void test_func() {
      parseR(42);  // ok, T deduced as int
      int i = 0;
      // parseR(i);  // error: T deduced as int&, which makes the declaration of x in parseR() invalid
    }
    
    
  
  }
  

  class Tmp {
  public:
  
  };

}

int
//main()
main_passing_by_reference()
{
  ch7_2::case3::test_func();

  return 0;
}
