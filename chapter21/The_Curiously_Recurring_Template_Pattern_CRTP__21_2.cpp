//
// Created by yangfeng on 8/28/2022.
//

#include "../headers.h"

using namespace std;

/*
 21.2 The Curiously Recurring Template Pattern (CRTP)
另一种模式是 CRTP。这一个有着奇怪名称的模式指的是将派生类作为模板参数传递给其某
个基类的一类技术。该模式的一种最简单的 C++实现方式如下：
template<typename Derived>
class CuriousBase {
...
};
class Curious : public CuriousBase<Curious> {
...
};
五车书馆
273
上面的 CRTP 的例子使用了非依赖性基类（nondependent base class 参见 13.4 节）：Curious
不是一个模板类，因此它对在依赖性基类中遇到的名称可见性问题是免疫的。但是这并不是
CRTP 的固有特征。事实上，我们同样可以使用下面的这一实现方式：
template<typename Derived>
class CuriousBase {
...
};
template<typename T>
class CuriousTemplate : public CuriousBase<CuriousTemplate<T>> {
...
};
将派生类通过模板参数传递给其基类，基类可以在不使用虚函数的情况下定制派生类的行
为。这使得 CRTP 对那些只能被实现为成员函数的情况（比如构造函数，析构函数，以及下
表运算符）或者依赖于派生类的特性的情况很有帮助（This makes CRTP useful to factor out
implementations that can only be member functions (e.g., constructor, destructors, and subscript
operators) or are dependent on the derived class’s identity.）。
一个 CRTP 的简单应用是将其用于追踪从一个 class 类型实例化出了多少对象。这一功能也可
以通过在构造函数中递增一个 static 数据成员、并在析构函数中递减该数据成员来实现。但
是给不同的 class 都提供相同的代码是一件很无聊的事情，而通过一个基类（非 CRTP）实现
这一功能又会将不同派生类实例的数目混杂在一起。事实上，可以实现下面这一模板：
#include <cstddef>
template<typename CountedType>
class ObjectCounter {
private:
inline static std::size_t count = 0; // number of existing objects
protected:
// default constructor
ObjectCounter() {
++count;
}
// copy constructor
ObjectCounter (ObjectCounter<CountedType> const&) {
++count;
}
// move constructor
ObjectCounter (ObjectCounter<CountedType> &&) {
++count;
}
// destructor
~ObjectCounter() {
五车书馆
274
--count;
}
public:
// return number of existing objects:
static std::size_t live() {
return count;
}
};
注意这里为了能够在 class 内部初始化 count 成员，使用了 inline。在 C++17 之前，必须在 class
模板外面定义它：
template<typename CountedType>
class ObjectCounter {
private:
static std::size_t count; // number of existing objects
...
};
// initialize counter with zero:
template<typename CountedType>
std::size_t ObjectCounter<CountedType>::count = 0;
当我们想要统计某一个 class 的对象（未被销毁）数目时，只需要让其派生自 ObjectCounter
即可。比如，可以按照下面的方式统计 MyString 的对象数目：
#include "objectcounter.hpp"
#include <iostream>
template<typename CharT>
class MyString : public ObjectCounter<MyString<CharT>> {
...
};
int main()
{
MyString<char> s1, s2;
MyString<wchar_t> ws;
std::cout << "num of MyString<char>: "
<< MyString<char>::live() << '\n';
std::cout << "num of MyString<wchar_t>: "
<< ws.live() << '\n';
}
21.2.1 The Barton-Nackman Trick
五车书馆
275
在 1994 年，John J.Barton 和 Lee R.Nackman 提出了一种被称为 restricted template expansion
的技术。该技术产生的动力之一是：在当时，函数模板的重载是严重受限的，而且 namespace
在当时也不为大多数编译器所支持。
为了说明这一技术，假设我们有一个需要为之定义 operator ==的类模板 Array。一个可能的
方案是将该运算符定义为类模板的成员，但是由于其第一个参数（绑定到 this 指针上的参数）
和第二个参数的类型转换规则不同（为什么？一个是指针？一个是 Arry 类型？）。由于我
们希望 operator ==对其参数是对称的，因此更倾向与将其定义为某一个 namespace 中的函
数。一种很直观的实现方式可能会像下面这样：
template<typename T>
class Array {
public:
...
};
template<typename T>bool operator== (Array<T> const& a, Array<T> const&
b)
{
...
}
不过如果函数模板不可以被重载的话，这会引入一个问题：在当前作用域内不可以再声明其
它的 operator ==模板，而其它的类模板却又很可能需要这样一个类似的模板。Barton 和
Nackman 通过将 operator ==定义成 class 内部的一个常规友元函数解决了这一问题：
template<typename T>
class Array {
static bool areEqual(Array<T> const& a, Array<T> const& b);
public:
...
friend bool operator== (Array<T> const& a, Array<T> const& b)
{
return areEqual(a, b);
}
};
假设我们用 float 实例化了该 Array 类。作为实例化的结果，该友元运算符函数也会被连带声
明，但是请注意该函数本身并不是一个函数模板的实例。作为实例化过程的一个副产品，它
是一个被注入到全局作用域的常规非模板函数。由于它是非模板函数，即使在重载函数模板
的功能被引入之前，也可以用其它的 operator ==对其进行重载。由于这样做避免了去定义一
个适用于所有类型 T 的 operator ==(T, T)模板，Barton 和 Nackman 将其称为 restricted template
expansion。
由于
operator== (Array<T> const&, Array<T> const&)
五车书馆
276
被定义在一个 class 的定义中，它会被隐式地当作 inline 函数，因此我们决定将其实现委托
给一个 static 成员函数（不需要是 inline 的）。
从 1994 年开始，friend 函数定义的查找方式就已经变了，因此在标准 C++中，Barton-Nackman
的方法就不再那么有用了。在其刚被发明出来的时候，如果要通过 friend name injection 实
例化模板，就需要 friend 函数的声明在类模板的闭合作用域内是可见的。而标准 C++则通过
参数依赖（argument-dependent lookup，参见第 13.2.2 节）来查找 friend 函数。这意味着在
函数的调用参数中，至少要有一个参数需要有一个包含了 friend 函数的关联类。如果参数的
类型是无关的 class 类型，即使该类型可以被转成包含了 friend 函数的 class 类型，也无法找
到该 friend 函数。比如：
class S {
};
template<typename T>
class Wrapper {
private:
T object;
public:
Wrapper(T obj) : object(obj) { //implicit conversion from T to Wrapper<T>
}
friend void foo(Wrapper<T> const&) {
}
};
int main()
{
S s;
Wrapper<S> w(s);
foo(w); // OK: Wrapper<S> is a class associated withw
foo(s); // ERROR: Wrapper<S> is not associated with s
}
此处的 foo(w)调用是有效的，因为 foo()是被定义于 Wrapper<S>中的友元，而 Wrapper<s>又
是与参数 w 有关的类。但是在 foo(s)的调用中，friend foo(Wrapper<S> const &)的声明并不可
见，这是因为定义了 foo(Wrapper<S> const &)的类 Wrapper<S>并没有和 S 类型的参数 s 关联
起来。因此，虽然在类型 S 和 Wrapper<S>之间有一个隐式的类型转换（通过 Wrapper<S>的
构造函数），但是由于一开始就没有找到这个 foo()函数，所以这个转换函数永远都不会被
考虑。而在 Barton 和 Nackman 发明它们这个方法的时候，friend 名称注射机制会让 freind foo()
可见，因此也就可以成功调用 foo(s)。
在 modern C++中，相比于直接定义一个函数模板，在类模板中定义一个 friend 函数的好处
是：友元函数可以访问该类模板的 private 成员以及 protected 成员，并且无需再次申明该类
五车书馆
277
模板的所有模板参数。但是，在与 Curiously Recurring Template Pattern(CRTP)结合之后，friend
函数定义可以变的更有用一些，就如在下面一节中节介绍的那样。
21.2.2 运算符的实现（Operator Implementations）
在给一个类重载运算符的时候，通常也需要重载一些其它的（当然也是相关的）运算符。比
如，一个实现了 operator ==的类，通常也会实现 operator !=，一个实现了 operator <的类，
通常也会实现其它的关系运算符（>，<=，>=）。在很多情况下，这些运算符中只有一个运
算符的定义比较有意思，其余的运算符都可以通过它来定义。例如，类 X 的 operator !=可以
通过使用 operator ==来定义：
bool operator!= (X const& x1, X const& x2) {
return !(x1 == x2);
}
对于那些 operator !=的定义类似的类型，可以通过模板将其泛型化：
template<typename T>
bool operator!= (T const& x1, T const& x2) {
return !(x1 == x2);
}
事实上，在 C++标准库的<utility>头文件中已经包含了类似的定义。但是，一些别的定义（比
如！=， >，<=和>=）在标准化过程中则被放到了 namespace std::rel_ops 中，因为当时可以
确定如果让它们在 std 中可见的话，会导致一些问题。实际上，如果让这些定义可见的话，
会使得任意类型都有一个!= operator (虽然实例化有可能失败)，而且对于其两个参数而言该
operator 也总会是最匹配的。
虽然上述第一个问题可以通过 SFINAE 技术解决（参见 19.4 节），这样的话这个!= operator
的定义只会在某种类型有合适的== operator 时才会被进行相应的实例化。但是第二个问题依
然存在：相比于用户定义的需要进行从派生类到基类的转化的!= operator，上述通用的!=
operator 定义总是会被优先选择，这有时会导致意料之外的结果。
另一种基于 CRTP 的运算符模板形式，则允许程序去选择泛型的运算符定义（假设为了增加
对代码的重用不会引入过度泛型化的问题）：
template<typename Derived>
class EqualityComparable
{
public:
friend bool operator!= (Derived const& x1, Derived const& x2)
{
return !(x1 == x2);
}
};
五车书馆
278
class X : public EqualityComparable<X>
{
public:
friend bool operator== (X const& x1, X const& x2) {
// implement logic for comparing two objects of type X
}
};
int main()
{
X x1, x2;
if (x1 != x2) { }
}
此处我们结合使用了 CRTP 和 Barton-Nackman 技术。EqualityComparable<>为了基于派生类
中定义的 operator==给其派生类提供 operator !=，使用了 CRTP。事实上这一定义是通过 friend
函数定义的形式提供的（Barton-Nackman 技术），这使得两个参数在类型转换时的 operator !=
行为一致。
当需要将一部分行为分解放置到基类中，同时需要保存派生类的标识时，CRTP 会很有用。
结合 Barton-Nackman，CRTP 可以基于一些简单的运算符为大量的运算符提供统一的定义。
这些特性使得 CRTP 和 Barton-Nackman 技术被 C++模板库的开发者所钟爱。
21.2.3 Facades
将 CRTP 和 Barton-Nackman 技术用于定义某些运算符是一种很简便的方式。我们可以更近一
步，这样 CRTP 基类就可以通过由 CRTP 派生类暴露出来的相对较少（但是会更容易实现）
的接口，来定义大部分甚至是全部 public 接口。这一被称为 facade 模式的技术，在定义需
要支持一些已有接口的新类型（数值类型，迭代器，容器等）时非常有用。
为了展示 facade 模式，我们为迭代器实现了一个 facade，这样可以大大简化一个符合标准
库要求的迭代器的编写。一个迭代器类型（尤其是 random access iterator）所需要支持的接
口是非常多的。下面的一个基础版的 IteratorFacade 模板展示了对迭代器接口的要求：
template<typename Derived, typename Value, typename Category,
typename Reference = Value&, typename Distance = std::ptrdiff_t>
class IteratorFacade
{
public:
using value_type = typename std::remove_const<Value>::type;
using reference = Reference;
using pointer = Value*;
using difference_type = Distance;
using iterator_category = Category;
五车书馆
279
// input iterator interface:
reference operator *() const { ... }
pointer operator ->() const { ... }
Derived& operator ++() { ... }
Derived operator ++(int) { ... }
friend bool operator== (IteratorFacade const& lhs,
IteratorFacade const& rhs) { ... }
...
// bidirectional iterator interface:
Derived& operator --() { ... }
Derived operator --(int) { ... }
// random access iterator interface:
reference operator [](difference_type n) const { ... }
Derived& operator +=(difference_type n) { ... }
...
friend difference_type operator -(IteratorFacade const& lhs,
IteratorFacade const& rhs) {
...
}
friend bool operator <(IteratorFacade const& lhs,
IteratorFacade const& rhs) { ... }
...
};
为了简洁，上面代码中已经省略了一部分声明，但是即使只是给每一个新的迭代器实现上述
代码中列出的接口，也是一件很繁杂的事情。幸运的是，可以从这些接口中提炼出一些核心
的运算符：

对于所有的迭代器，都有如下运算符：

解引用（dereference）：访问由迭代器指向的值（通常是通过 operator *和->）。

递增（increment）：移动迭代器以让其指向序列中的下一个元素。

相等（equals）：判断两个迭代器指向的是不是序列中的同一个元素。

对于双向迭代器，还有：

递减（decrement）：移动迭代器以让其指向列表中的前一个元素。

对于随机访问迭代器，还有：

前进（advance）：将迭代器向前或者向后移动 n 步。

测距（measureDistance）：测量一个序列中两个迭代器之间的距离。
五车书馆
280
Facade 的作用是给一个只实现了核心运算符（core operations）的类型提供完整的迭代器接
口。IteratorFacade 的实现就涉及到到将迭代器语法映射到最少量的接口上。在下面的例子
中，我们通过成员函数 asDerived()访问 CRTP 派生类：
Derived& asDerived() {
return *static_cast<Derived*>(this);
}
Derived const& asDerived() const {
return *static_cast<Derived const*>(this);
}
有了以上定义，facade 中大部分功能的实现就变得很直接了。下面只展示一部分的迭代器接
口，其余的实现都很类似：
reference operator*() const {
return asDerived().dereference();
}
Derived& operator++() {
asDerived().increment();
return asDerived();
}
Derived operator++(int) {
Derived result(asDerived());
asDerived().increment();
return result;
}
friend bool operator== (IteratorFacade const& lhs, IteratorFacade const& rhs) {
return lhs.asDerived().equals(rhs.asDerived());
}
定义一个链表的迭代器
结合以上 IteratorFacade 的定义，可以容易地定义一个指向简单链表的迭代器。比如，链表
中节点的定义如下：
template<typename T>
class ListNode
{
public:
T value;
ListNode<T>* next = nullptr;
五车书馆
281
~ListNode() { delete next; }
};
通过使用 IteratorFacade，可以以一种很直接的方式定义指向这样一个链表的迭代器：
template<typename T>
class ListNodeIterator
: public IteratorFacade<ListNodeIterator<T>, T,
std::forward_iterator_tag>
{
ListNode<T>* current = nullptr;
public:
T& dereference() const {
return current->value;
}
void increment() {
current = current->next;
}
bool equals(ListNodeIterator const& other) const {
return current == other.current;
}
ListNodeIterator(ListNode<T>* current = nullptr) :
current(current) { }
};
ListNodeIterator 在使用很少量代码的情况下，提供了一个前向迭代器（forward iterator）所
需要的所有运算符和嵌套类型。接下来会看到，即使是实现一个比较复杂的迭代器（比如，
随机访问迭代器），也只需要再额外执行少量的工作。
隐藏接口
上述 ListNodeIterator 实现的一个缺点是，需要将 dereference()，advance()和 equals()运算符
暴露成 public 接口。为了避免这一缺点，可以重写 IteratorFacade：通过一个单独的访问类
（access class），来执行其所有作用于 CRTP 派生类的运算符操作。我们称这个访问类为
IteratorFacadeAccess：
// ‘friend’ this class to allow IteratorFacade access to core iterator operations:
class IteratorFacadeAccess
{
// only IteratorFacade can use these definitions
template<typename Derived, typename Value, typename Category,
typename Reference, typename Distance>
五车书馆
282
friend class IteratorFacade;
// required of all iterators:
template<typename Reference, typename Iterator>
static Reference dereference(Iterator const& i) {
return i.dereference();
}
...
// required of bidirectional iterators:
template<typename Iterator>
static void decrement(Iterator& i) {
return i.decrement();
}
// required of random-access iterators:
template<typename Iterator, typename Distance>
static void advance(Iterator& i, Distance n) {
return i.advance(n);
}
...
};
该 class 为每一个核心迭代器操作都提供了对应的 static 成员函数，它们会调用迭代器中相
应的（nonstatic）成员函数。所有的 static 成员函数都是 private 的，只有 IteratorFacade 才
可以访问它们。因此，我们的 ListNodeIterator 可以将 IteratorFacadeAccess 当作 friend，并把
facade 所需要的接口继续保持为 private 的：
friend class IteratorFacadeAccess;
迭代器的适配器（Iterator Adapters）
使用我们的 IteratorFacade 可以很容易的创建一个迭代器的适配器，这样就可以基于已有的
迭代器生成一个提供了对底层序列进行了视角转换的新的迭代器。比如，可能有一个存储了
Person 类型数值的容器：
struct Person {
std::string firstName;
std::string lastName;
friend std::ostream& operator<<(std::ostream& strm, Person const& p) {
return strm << p.lastName << ", " << p.firstName;
}
};
五车书馆
283
但是，相对于编译容器中所有 Person 元素的值，我们可能只是想得到其 first name。在本节
中我们会开发一款迭代器的适配器（称之为 ProjectionIterator），通过它可以将底层迭代器
（base）“投射”到一些指向数据成员的指针（pointer-to-data member）上，比如：
Person::firstName。
ProjectionIterator 是依据基类迭代器以及将要被迭代器暴露的数值类型定义的一种迭代器：
template<typename Iterator, typename T>
class ProjectionIterator
: public IteratorFacade<ProjectionIterator<Iterator, T>, T, typename
std::iterator_traits<Iterator>::iterator_category, T&, typename
std::iterator_traits<Iterator>::difference_type>
{
using Base = typename std::iterator_traits<Iterator>::value_type;
using Distance = typename std::iterator_traits<Iterator>::difference_type;
Iterator iter;
T Base::* member;
friend class IteratorFacadeAccess
...
//implement core iterator operations for IteratorFacade
public:
ProjectionIterator(Iterator iter, T Base::* member)
: iter(iter), member(member) { }
};
template<typename Iterator, typename Base, typename T>
auto project(Iterator iter, T Base::* member) {
return ProjectionIterator<Iterator, T>(iter, member);
}
每一个 projection iterator 都定存储了两个值：iter（指向底层序列的迭代器），以及 member
（一个指向数据成员的指针，表示将要投射到的成员）。有了这一认知，我们来考虑传递给
基类 IteratorFacade 的模板参数。第一个是 ProjectionIterator 本身（为了使用 CRTP）。第二
个参数（T）和第四个参数（T&）是我们的 projection iterator 的数值和引用类型，将其定义
成 T 类型数值的序列。第三和第五个参数仅仅只是传递了底层迭代器的种类的不同类型。因
此，如果 Iterator 是 input iterator 的话，我们的 projection iterator 也将是 input iterator，如
果 Iterator 是双向迭代器的话，我们的 projection iterator 也将是双向迭代器，以此类推。
Project()函数则使得 projection iterator 的构建变得很简单。
唯一缺少的是对 IteratorFacade 核心需求的实现。最有意思的是 dereference()，它会解引用
底层迭代器并投射到指向数据成员的指针：
T& dereference() const {
return (*iter).*member;
}
五车书馆
284
其余操作是依照底层迭代器实现的：
void increment() {
++iter;
}
bool equals(ProjectionIterator const& other) const {
return iter == other.iter;
}
void decrement() {
--iter;
}
为了简单起见，我们忽略了对随机访问迭代器的定义，但是其实现是类似的。
就这些！通过使用 projection iterator，我们可以打印出存储在 vector 中的 Person 数值的 first
name：
#include <vector>
#include <algorithm>
#include <iterator>
int main()
{
std::vector<Person> authors = { {"David", "Vandevoorde"},
{"Nicolai", "Josuttis"},
{"Douglas", "Gregor"} };
std::copy(project(authors.begin(), &Person::firstName),
project(authors.end(), &Person::firstName),
std::ostream_iterator<std::string>(std::cout, "\n"));
}
Facade 模式在创建需要符合特定接口的新类型时异常有用。新的类型只需要向 facade 暴露
出少量和核心操作，后续 facade 会通过结合使用 CRTP 和 Barton-Nackman 技术提供完整且
正确的 public 接口。
 */

namespace ch21_2 {

  namespace case1 {
    template<typename Derived>
    class CuriousBase { };
    
    class Curious : public CuriousBase<Curious> { };
    
    void test() {
      cout << "hello, world" << endl;
    }
    
  }
  
  namespace case2 {
    template<typename Derived>
    class CuriousBase { };
    
    template<typename T>
    class CuriousTemplate : public CuriousBase<CuriousTemplate<T>> { };
    
    void test() {
    
    }
    
  }
  
  /*
   一个 CRTP 的简单应用是将其用于追踪从一个 class 类型实例化出了多少对象。这一功能也可
以通过在构造函数中递增一个 static 数据成员、并在析构函数中递减该数据成员来实现。但
是给不同的 class 都提供相同的代码是一件很无聊的事情，而通过一个基类（非 CRTP）实现
这一功能又会将不同派生类实例的数目混杂在一起。事实上，可以实现下面这一模板：
   */
  
  namespace case3 {
    
    template<typename CountedType>
    class ObjectCounter {
    private:
      /*
       注意这里为了能够在 class 内部初始化 count 成员，使用了 inline。在 C++17 之前，必须在 class
模板外面定义它：
template<typename CountedType>
class ObjectCounter {
private:
static std::size_t count; // number of existing objects
...
};
// initialize counter with zero:
template<typename CountedType>
std::size_t ObjectCounter<CountedType>::count = 0;
       */
      inline static std::size_t count = 0; // number of existing objects
      
    protected:
      
      // default constructor
      ObjectCounter() { ++count; }
      
      // copy constructor
      ObjectCounter (ObjectCounter<CountedType> const&) { ++count; }
      
      // move constructor
      ObjectCounter (ObjectCounter<CountedType> &&) { ++count; }
      
      // destructor
      ~ObjectCounter() { --count; }
      
    public:
      
      // return number of existing objects:
      static std::size_t live() { return count; }
    };
    
    /*
    当我们想要统计某一个 class 的对象（未被销毁）数目时，只需要让其派生自 ObjectCounter
即可。比如，可以按照下面的方式统计 MyString 的对象数目：
#include "objectcounter.hpp"
#include <iostream>
template<typename CharT>
class MyString : public ObjectCounter<MyString<CharT>> {
...
};
int main()
{
MyString<char> s1, s2;
MyString<wchar_t> ws;
std::cout << "num of MyString<char>: "
<< MyString<char>::live() << '\n';
std::cout << "num of MyString<wchar_t>: "
<< ws.live() << '\n';
}
     */

    template<typename CharT>
    class MyString : public ObjectCounter<MyString<CharT>> { };
    
    void test() {
      MyString<char> s1, s2;
      MyString<wchar_t> ws;
      std::cout << "num of MyString<char>: " << MyString<char>::live() << '\n';
      std::cout << "num of MyString<wchar_t>: " << ws.live() << '\n';
    }
    
  }
  
  /*
   21.2.1 The Barton-Nackman Trick
五车书馆
275
在 1994 年，John J.Barton 和 Lee R.Nackman 提出了一种被称为 restricted template expansion
的技术。该技术产生的动力之一是：在当时，函数模板的重载是严重受限的，而且 namespace
在当时也不为大多数编译器所支持。
为了说明这一技术，假设我们有一个需要为之定义 operator ==的类模板 Array。一个可能的
方案是将该运算符定义为类模板的成员，但是由于其第一个参数（绑定到 this 指针上的参数）
和第二个参数的类型转换规则不同（为什么？一个是指针？一个是 Arry 类型？）。由于我
们希望 operator ==对其参数是对称的，因此更倾向与将其定义为某一个 namespace 中的函
数。一种很直观的实现方式可能会像下面这样：
template<typename T>
class Array {
public:
...
};
template<typename T>bool operator== (Array<T> const& a, Array<T> const&
b)
{
...
}
不过如果函数模板不可以被重载的话，这会引入一个问题：在当前作用域内不可以再声明其
它的 operator ==模板，而其它的类模板却又很可能需要这样一个类似的模板。Barton 和
Nackman 通过将 operator ==定义成 class 内部的一个常规友元函数解决了这一问题：
template<typename T>
class Array {
static bool areEqual(Array<T> const& a, Array<T> const& b);
public:
...
friend bool operator== (Array<T> const& a, Array<T> const& b)
{
return areEqual(a, b);
}
};
假设我们用 float 实例化了该 Array 类。作为实例化的结果，该友元运算符函数也会被连带声
明，但是请注意该函数本身并不是一个函数模板的实例。作为实例化过程的一个副产品，它
是一个被注入到全局作用域的常规非模板函数。由于它是非模板函数，即使在重载函数模板
的功能被引入之前，也可以用其它的 operator ==对其进行重载。由于这样做避免了去定义一
个适用于所有类型 T 的 operator ==(T, T)模板，Barton 和 Nackman 将其称为 restricted template
expansion。
由于
operator== (Array<T> const&, Array<T> const&)
五车书馆
276
被定义在一个 class 的定义中，它会被隐式地当作 inline 函数，因此我们决定将其实现委托
给一个 static 成员函数（不需要是 inline 的）。
从 1994 年开始，friend 函数定义的查找方式就已经变了，因此在标准 C++中，Barton-Nackman
的方法就不再那么有用了。在其刚被发明出来的时候，如果要通过 friend name injection 实
例化模板，就需要 friend 函数的声明在类模板的闭合作用域内是可见的。而标准 C++则通过
参数依赖（argument-dependent lookup，参见第 13.2.2 节）来查找 friend 函数。这意味着在
函数的调用参数中，至少要有一个参数需要有一个包含了 friend 函数的关联类。如果参数的
类型是无关的 class 类型，即使该类型可以被转成包含了 friend 函数的 class 类型，也无法找
到该 friend 函数。比如：
class S {
};
template<typename T>
class Wrapper {
private:
T object;
public:
Wrapper(T obj) : object(obj) { //implicit conversion from T to Wrapper<T>
}
friend void foo(Wrapper<T> const&) {
}
};
int main()
{
S s;
Wrapper<S> w(s);
foo(w); // OK: Wrapper<S> is a class associated withw
foo(s); // ERROR: Wrapper<S> is not associated with s
}
此处的 foo(w)调用是有效的，因为 foo()是被定义于 Wrapper<S>中的友元，而 Wrapper<s>又
是与参数 w 有关的类。但是在 foo(s)的调用中，friend foo(Wrapper<S> const &)的声明并不可
见，这是因为定义了 foo(Wrapper<S> const &)的类 Wrapper<S>并没有和 S 类型的参数 s 关联
起来。因此，虽然在类型 S 和 Wrapper<S>之间有一个隐式的类型转换（通过 Wrapper<S>的
构造函数），但是由于一开始就没有找到这个 foo()函数，所以这个转换函数永远都不会被
考虑。而在 Barton 和 Nackman 发明它们这个方法的时候，friend 名称注射机制会让 freind foo()
可见，因此也就可以成功调用 foo(s)。
在 modern C++中，相比于直接定义一个函数模板，在类模板中定义一个 friend 函数的好处
是：友元函数可以访问该类模板的 private 成员以及 protected 成员，并且无需再次申明该类
五车书馆
277
模板的所有模板参数。但是，在与 Curiously Recurring Template Pattern(CRTP)结合之后，friend
函数定义可以变的更有用一些，就如在下面一节中节介绍的那样。
   */
  
  namespace case4 {
    
    template<typename T>
    class Array {
    public:
    };
    
    template<typename T>
    bool operator== (Array<T> const& a, Array<T> const& b) {
      return a == b;
    }
    
    void test() {
    
    }
    
  }
  
  namespace case5 {
    
    template<typename T>
    class Array {
      static bool areEqual(Array<T> const& a, Array<T> const& b);
      
    public:
      friend bool operator== (Array<T> const& a, Array<T> const& b) { return areEqual(a, b); }
      
    };
    
    void test() {
    
    }
    
  }
  
  /*
   从 1994 年开始，friend 函数定义的查找方式就已经变了，因此在标准 C++中，Barton-Nackman
的方法就不再那么有用了。在其刚被发明出来的时候，如果要通过 friend name injection 实
例化模板，就需要 friend 函数的声明在类模板的闭合作用域内是可见的。而标准 C++则通过
参数依赖（argument-dependent lookup，参见第 13.2.2 节）来查找 friend 函数。这意味着在
函数的调用参数中，至少要有一个参数需要有一个包含了 friend 函数的关联类。如果参数的
类型是无关的 class 类型，即使该类型可以被转成包含了 friend 函数的 class 类型，也无法找
到该 friend 函数。比如：
   */
  
  namespace case6 {
    
    class S { };
    
    template<typename T>
    class Wrapper {
    private:
      T object;
      
    public:
      Wrapper(T obj) : object(obj) {} //implicit conversion from T to Wrapper<T>
      friend void foo(Wrapper<T> const&) { }
      
    };
    
    void test()
    {
      S s;
      Wrapper<S> w(s);
      foo(w); // OK: Wrapper<S> is a class associated withw
      // foo(s); // ERROR: Wrapper<S> is not associated with s
    }
    
    /*
     在 modern C++中，相比于直接定义一个函数模板，在类模板中定义一个 friend 函数的好处
是：友元函数可以访问该类模板的 private 成员以及 protected 成员，并且无需再次申明该类
模板的所有模板参数。但是，在与 Curiously Recurring Template Pattern(CRTP)结合之后，friend
函数定义可以变的更有用一些，就如在下面一节中节介绍的那样。
     */
    
  }
  
  /*
   21.2.2 运算符的实现（Operator Implementations）
在给一个类重载运算符的时候，通常也需要重载一些其它的（当然也是相关的）运算符。比
如，一个实现了 operator ==的类，通常也会实现 operator !=，一个实现了 operator <的类，
通常也会实现其它的关系运算符（>，<=，>=）。在很多情况下，这些运算符中只有一个运
算符的定义比较有意思，其余的运算符都可以通过它来定义。例如，类 X 的 operator !=可以
通过使用 operator ==来定义：
bool operator!= (X const& x1, X const& x2) {
return !(x1 == x2);
}
对于那些 operator !=的定义类似的类型，可以通过模板将其泛型化：
template<typename T>
bool operator!= (T const& x1, T const& x2) {
return !(x1 == x2);
}
事实上，在 C++标准库的<utility>头文件中已经包含了类似的定义。但是，一些别的定义（比
如！=， >，<=和>=）在标准化过程中则被放到了 namespace std::rel_ops 中，因为当时可以
确定如果让它们在 std 中可见的话，会导致一些问题。实际上，如果让这些定义可见的话，
会使得任意类型都有一个!= operator (虽然实例化有可能失败)，而且对于其两个参数而言该
operator 也总会是最匹配的。
虽然上述第一个问题可以通过 SFINAE 技术解决（参见 19.4 节），这样的话这个!= operator
的定义只会在某种类型有合适的== operator 时才会被进行相应的实例化。但是第二个问题依
然存在：相比于用户定义的需要进行从派生类到基类的转化的!= operator，上述通用的!=
operator 定义总是会被优先选择，这有时会导致意料之外的结果。
另一种基于 CRTP 的运算符模板形式，则允许程序去选择泛型的运算符定义（假设为了增加
对代码的重用不会引入过度泛型化的问题）：
template<typename Derived>
class EqualityComparable
{
public:
friend bool operator!= (Derived const& x1, Derived const& x2)
{
return !(x1 == x2);
}
};
五车书馆
278
class X : public EqualityComparable<X>
{
public:
friend bool operator== (X const& x1, X const& x2) {
// implement logic for comparing two objects of type X
}
};
int main()
{
X x1, x2;
if (x1 != x2) { }
}
此处我们结合使用了 CRTP 和 Barton-Nackman 技术。EqualityComparable<>为了基于派生类
中定义的 operator==给其派生类提供 operator !=，使用了 CRTP。事实上这一定义是通过 friend
函数定义的形式提供的（Barton-Nackman 技术），这使得两个参数在类型转换时的 operator !=
行为一致。
当需要将一部分行为分解放置到基类中，同时需要保存派生类的标识时，CRTP 会很有用。
结合 Barton-Nackman，CRTP 可以基于一些简单的运算符为大量的运算符提供统一的定义。
这些特性使得 CRTP 和 Barton-Nackman 技术被 C++模板库的开发者所钟爱。
   */
  
  namespace case7 {
    
    template<typename Derived>
    class EqualityComparable
    {
    public:
      friend bool operator!= (Derived const& x1, Derived const& x2) { return !(x1 == x2); }
      
    };
    
    class X : public EqualityComparable<X>
    {
    public:
      friend bool operator== (X const& x1, X const& x2) {  // implement logic for comparing two objects of type X
        return x1 == x2;  // my implement for cancling the warning
      }
      
    };
    
    /*
     此处我们结合使用了 CRTP 和 Barton-Nackman 技术。EqualityComparable<>为了基于派生类
中定义的 operator==给其派生类提供 operator !=，使用了 CRTP。事实上这一定义是通过 friend
函数定义的形式提供的（Barton-Nackman 技术），这使得两个参数在类型转换时的 operator !=
行为一致。
当需要将一部分行为分解放置到基类中，同时需要保存派生类的标识时，CRTP 会很有用。
结合 Barton-Nackman，CRTP 可以基于一些简单的运算符为大量的运算符提供统一的定义。
这些特性使得 CRTP 和 Barton-Nackman 技术被 C++模板库的开发者所钟爱。
     */
    
    void test() {
      X x1, x2;
      if (x1 != x2) { }
    
    }
    
  }
  
  /*
   21.2.3 Facades
将 CRTP 和 Barton-Nackman 技术用于定义某些运算符是一种很简便的方式。我们可以更近一
步，这样 CRTP 基类就可以通过由 CRTP 派生类暴露出来的相对较少（但是会更容易实现）
的接口，来定义大部分甚至是全部 public 接口。这一被称为 facade 模式的技术，在定义需
要支持一些已有接口的新类型（数值类型，迭代器，容器等）时非常有用。
为了展示 facade 模式，我们为迭代器实现了一个 facade，这样可以大大简化一个符合标准
库要求的迭代器的编写。一个迭代器类型（尤其是 random access iterator）所需要支持的接
口是非常多的。下面的一个基础版的 IteratorFacade 模板展示了对迭代器接口的要求：
template<typename Derived, typename Value, typename Category,
typename Reference = Value&, typename Distance = std::ptrdiff_t>
class IteratorFacade
{
public:
using value_type = typename std::remove_const<Value>::type;
using reference = Reference;
using pointer = Value*;
using difference_type = Distance;
using iterator_category = Category;
五车书馆
279
// input iterator interface:
reference operator *() const { ... }
pointer operator ->() const { ... }
Derived& operator ++() { ... }
Derived operator ++(int) { ... }
friend bool operator== (IteratorFacade const& lhs,
IteratorFacade const& rhs) { ... }
...
// bidirectional iterator interface:
Derived& operator --() { ... }
Derived operator --(int) { ... }
// random access iterator interface:
reference operator [](difference_type n) const { ... }
Derived& operator +=(difference_type n) { ... }
...
friend difference_type operator -(IteratorFacade const& lhs,
IteratorFacade const& rhs) {
...
}
friend bool operator <(IteratorFacade const& lhs,
IteratorFacade const& rhs) { ... }
...
};
为了简洁，上面代码中已经省略了一部分声明，但是即使只是给每一个新的迭代器实现上述
代码中列出的接口，也是一件很繁杂的事情。幸运的是，可以从这些接口中提炼出一些核心
的运算符：

对于所有的迭代器，都有如下运算符：

解引用（dereference）：访问由迭代器指向的值（通常是通过 operator *和->）。

递增（increment）：移动迭代器以让其指向序列中的下一个元素。

相等（equals）：判断两个迭代器指向的是不是序列中的同一个元素。

对于双向迭代器，还有：

递减（decrement）：移动迭代器以让其指向列表中的前一个元素。

对于随机访问迭代器，还有：

前进（advance）：将迭代器向前或者向后移动 n 步。

测距（measureDistance）：测量一个序列中两个迭代器之间的距离。
五车书馆
280
Facade 的作用是给一个只实现了核心运算符（core operations）的类型提供完整的迭代器接
口。IteratorFacade 的实现就涉及到到将迭代器语法映射到最少量的接口上。在下面的例子
中，我们通过成员函数 asDerived()访问 CRTP 派生类：
Derived& asDerived() {
return *static_cast<Derived*>(this);
}
Derived const& asDerived() const {
return *static_cast<Derived const*>(this);
}
有了以上定义，facade 中大部分功能的实现就变得很直接了。下面只展示一部分的迭代器接
口，其余的实现都很类似：
reference operator*() const {
return asDerived().dereference();
}
Derived& operator++() {
asDerived().increment();
return asDerived();
}
Derived operator++(int) {
Derived result(asDerived());
asDerived().increment();
return result;
}
friend bool operator== (IteratorFacade const& lhs, IteratorFacade const& rhs) {
return lhs.asDerived().equals(rhs.asDerived());
}
定义一个链表的迭代器
结合以上 IteratorFacade 的定义，可以容易地定义一个指向简单链表的迭代器。比如，链表
中节点的定义如下：
template<typename T>
class ListNode
{
public:
T value;
ListNode<T>* next = nullptr;
五车书馆
281
~ListNode() { delete next; }
};
通过使用 IteratorFacade，可以以一种很直接的方式定义指向这样一个链表的迭代器：
template<typename T>
class ListNodeIterator
: public IteratorFacade<ListNodeIterator<T>, T,
std::forward_iterator_tag>
{
ListNode<T>* current = nullptr;
public:
T& dereference() const {
return current->value;
}
void increment() {
current = current->next;
}
bool equals(ListNodeIterator const& other) const {
return current == other.current;
}
ListNodeIterator(ListNode<T>* current = nullptr) :
current(current) { }
};
ListNodeIterator 在使用很少量代码的情况下，提供了一个前向迭代器（forward iterator）所
需要的所有运算符和嵌套类型。接下来会看到，即使是实现一个比较复杂的迭代器（比如，
随机访问迭代器），也只需要再额外执行少量的工作。
隐藏接口
上述 ListNodeIterator 实现的一个缺点是，需要将 dereference()，advance()和 equals()运算符
暴露成 public 接口。为了避免这一缺点，可以重写 IteratorFacade：通过一个单独的访问类
（access class），来执行其所有作用于 CRTP 派生类的运算符操作。我们称这个访问类为
IteratorFacadeAccess：
// ‘friend’ this class to allow IteratorFacade access to core iterator operations:
class IteratorFacadeAccess
{
// only IteratorFacade can use these definitions
template<typename Derived, typename Value, typename Category,
typename Reference, typename Distance>
五车书馆
282
friend class IteratorFacade;
// required of all iterators:
template<typename Reference, typename Iterator>
static Reference dereference(Iterator const& i) {
return i.dereference();
}
...
// required of bidirectional iterators:
template<typename Iterator>
static void decrement(Iterator& i) {
return i.decrement();
}
// required of random-access iterators:
template<typename Iterator, typename Distance>
static void advance(Iterator& i, Distance n) {
return i.advance(n);
}
...
};
该 class 为每一个核心迭代器操作都提供了对应的 static 成员函数，它们会调用迭代器中相
应的（nonstatic）成员函数。所有的 static 成员函数都是 private 的，只有 IteratorFacade 才
可以访问它们。因此，我们的 ListNodeIterator 可以将 IteratorFacadeAccess 当作 friend，并把
facade 所需要的接口继续保持为 private 的：
friend class IteratorFacadeAccess;
迭代器的适配器（Iterator Adapters）
使用我们的 IteratorFacade 可以很容易的创建一个迭代器的适配器，这样就可以基于已有的
迭代器生成一个提供了对底层序列进行了视角转换的新的迭代器。比如，可能有一个存储了
Person 类型数值的容器：
struct Person {
std::string firstName;
std::string lastName;
friend std::ostream& operator<<(std::ostream& strm, Person const& p) {
return strm << p.lastName << ", " << p.firstName;
}
};
五车书馆
283
但是，相对于编译容器中所有 Person 元素的值，我们可能只是想得到其 first name。在本节
中我们会开发一款迭代器的适配器（称之为 ProjectionIterator），通过它可以将底层迭代器
（base）“投射”到一些指向数据成员的指针（pointer-to-data member）上，比如：
Person::firstName。
ProjectionIterator 是依据基类迭代器以及将要被迭代器暴露的数值类型定义的一种迭代器：
template<typename Iterator, typename T>
class ProjectionIterator
: public IteratorFacade<ProjectionIterator<Iterator, T>, T, typename
std::iterator_traits<Iterator>::iterator_category, T&, typename
std::iterator_traits<Iterator>::difference_type>
{
using Base = typename std::iterator_traits<Iterator>::value_type;
using Distance = typename std::iterator_traits<Iterator>::difference_type;
Iterator iter;
T Base::* member;
friend class IteratorFacadeAccess
...
//implement core iterator operations for IteratorFacade
public:
ProjectionIterator(Iterator iter, T Base::* member)
: iter(iter), member(member) { }
};
template<typename Iterator, typename Base, typename T>
auto project(Iterator iter, T Base::* member) {
return ProjectionIterator<Iterator, T>(iter, member);
}
每一个 projection iterator 都定存储了两个值：iter（指向底层序列的迭代器），以及 member
（一个指向数据成员的指针，表示将要投射到的成员）。有了这一认知，我们来考虑传递给
基类 IteratorFacade 的模板参数。第一个是 ProjectionIterator 本身（为了使用 CRTP）。第二
个参数（T）和第四个参数（T&）是我们的 projection iterator 的数值和引用类型，将其定义
成 T 类型数值的序列。第三和第五个参数仅仅只是传递了底层迭代器的种类的不同类型。因
此，如果 Iterator 是 input iterator 的话，我们的 projection iterator 也将是 input iterator，如
果 Iterator 是双向迭代器的话，我们的 projection iterator 也将是双向迭代器，以此类推。
Project()函数则使得 projection iterator 的构建变得很简单。
唯一缺少的是对 IteratorFacade 核心需求的实现。最有意思的是 dereference()，它会解引用
底层迭代器并投射到指向数据成员的指针：
T& dereference() const {
return (*iter).*member;
}
五车书馆
284
其余操作是依照底层迭代器实现的：
void increment() {
++iter;
}
bool equals(ProjectionIterator const& other) const {
return iter == other.iter;
}
void decrement() {
--iter;
}
为了简单起见，我们忽略了对随机访问迭代器的定义，但是其实现是类似的。
就这些！通过使用 projection iterator，我们可以打印出存储在 vector 中的 Person 数值的 first
name：
#include <vector>
#include <algorithm>
#include <iterator>
int main()
{
std::vector<Person> authors = { {"David", "Vandevoorde"},
{"Nicolai", "Josuttis"},
{"Douglas", "Gregor"} };
std::copy(project(authors.begin(), &Person::firstName),
project(authors.end(), &Person::firstName),
std::ostream_iterator<std::string>(std::cout, "\n"));
}
Facade 模式在创建需要符合特定接口的新类型时异常有用。新的类型只需要向 facade 暴露
出少量和核心操作，后续 facade 会通过结合使用 CRTP 和 Barton-Nackman 技术提供完整且
正确的 public 接口。
   */
  
  namespace case8 {
  
    template<typename Derived, typename Value, typename Category, 
      typename Reference = Value&, typename Distance = std::ptrdiff_t>
    class IteratorFacade {
    public:
      using value_type = typename std::remove_const<Value>::type;
      using reference = Reference;
      using pointer = Value*;
      using difference_type = Distance;
      using iterator_category = Category;
      
      // input iterator interface:
      reference operator *() const {
        return asDerived().derefrence();
      }
      pointer operator ->() const {  }
      Derived& operator ++() {
        asDerived().increment();
        return asDerived();
      }
      Derived operator ++(int) {
        Derived result(asDerived());
        asDerived().increment();
        return result;
      }
      friend bool operator== (IteratorFacade const& lhs, IteratorFacade const& rhs) {
        return lhs.asDerived().equals(rhs.asDerived());
      }
      
      // bidirectional iterator interface:
      Derived& operator --() {  }
      Derived operator --(int) {  }
      
      // random access iterator interface:
      reference operator [](difference_type n) const {  }
      Derived& operator +=(difference_type n) {  }
  
      Derived& asDerived() {
        return *static_cast<Derived*>(this);
      }
  
      Derived const& asDerived() const {
        return *static_cast<Derived const*>(this);
      }
      
      friend difference_type operator -(IteratorFacade const& lhs, IteratorFacade const& rhs) { }
      friend bool operator <(IteratorFacade const& lhs, IteratorFacade const& rhs) { return lhs < rhs; }  // my inplement for calcle the warning
      
    };
    
    // 定义一个链表的迭代器
      //结合以上 IteratorFacade 的定义，可以容易地定义一个指向简单链表的迭代器。比如，链表
      //中节点的定义如下：
    template<typename T>
    class ListNode
    {
    public:
      T value;
      ListNode<T>* next = nullptr;
      ~ListNode() { delete next; }
    };
    
    // 通过使用 IteratorFacade，可以以一种很直接的方式定义指向这样一个链表的迭代器：
    template<typename T>
    class ListNodeIterator : public IteratorFacade<ListNodeIterator<T>, T, std::forward_iterator_tag>
    {
    private:
      ListNode<T>* current = nullptr;
      
    public:
      ListNodeIterator(ListNode<T>* current = nullptr) : current(current) { }
      
      T& dereference() const { return current->value; }
      
      void increment() { current = current->next; }
      
      bool equals(ListNodeIterator const& other) const { return current == other.current; }
      
      
    };
    
    /*
    隐藏接口
上述 ListNodeIterator 实现的一个缺点是，需要将 dereference()，advance()和 equals()运算符
暴露成 public 接口。为了避免这一缺点，可以重写 IteratorFacade：通过一个单独的访问类
（access class），来执行其所有作用于 CRTP 派生类的运算符操作。我们称这个访问类为
IteratorFacadeAccess：
     */
    // ‘friend’ this class to allow IteratorFacade access to core iterator operations:
    class IteratorFacadeAccess
    {
      // only IteratorFacade can use these definitions
      template<typename Derived, typename Value, typename Category,
        typename Reference, typename Distance>
      friend class IteratorFacade;
      
      // required of all iterators:
      template<typename Reference, typename Iterator>
      static Reference dereference(Iterator const& i) { return i.dereference(); }
      
      // required of bidirectional iterators:
      template<typename Iterator>
      static void decrement(Iterator& i) { return i.decrement(); }
      
      // required of random-access iterators:
      template<typename Iterator, typename Distance>
      static void advance(Iterator& i, Distance n) { return i.advance(n); }
    };
    
    void test() {
    
    }
    
  }
  
  /*
   迭代器的适配器（Iterator Adapters）
使用我们的 IteratorFacade 可以很容易的创建一个迭代器的适配器，这样就可以基于已有的
迭代器生成一个提供了对底层序列进行了视角转换的新的迭代器。比如，可能有一个存储了
Person 类型数值的容器：
struct Person {
std::string firstName;
std::string lastName;
friend std::ostream& operator<<(std::ostream& strm, Person const& p) {
return strm << p.lastName << ", " << p.firstName;
}
};
五车书馆
283
但是，相对于编译容器中所有 Person 元素的值，我们可能只是想得到其 first name。在本节
中我们会开发一款迭代器的适配器（称之为 ProjectionIterator），通过它可以将底层迭代器
（base）“投射”到一些指向数据成员的指针（pointer-to-data member）上，比如：
Person::firstName。
ProjectionIterator 是依据基类迭代器以及将要被迭代器暴露的数值类型定义的一种迭代器：
template<typename Iterator, typename T>
class ProjectionIterator
: public IteratorFacade<ProjectionIterator<Iterator, T>, T, typename
std::iterator_traits<Iterator>::iterator_category, T&, typename
std::iterator_traits<Iterator>::difference_type>
{
using Base = typename std::iterator_traits<Iterator>::value_type;
using Distance = typename std::iterator_traits<Iterator>::difference_type;
Iterator iter;
T Base::* member;
friend class IteratorFacadeAccess
…
//implement core iterator operations for IteratorFacade
public:
ProjectionIterator(Iterator iter, T Base::* member)
: iter(iter), member(member) { }
};
template<typename Iterator, typename Base, typename T>
auto project(Iterator iter, T Base::* member) {
return ProjectionIterator<Iterator, T>(iter, member);
}
每一个 projection iterator 都定存储了两个值：iter（指向底层序列的迭代器），以及 member
（一个指向数据成员的指针，表示将要投射到的成员）。有了这一认知，我们来考虑传递给
基类 IteratorFacade 的模板参数。第一个是 ProjectionIterator 本身（为了使用 CRTP）。第二
个参数（T）和第四个参数（T&）是我们的 projection iterator 的数值和引用类型，将其定义
成 T 类型数值的序列。第三和第五个参数仅仅只是传递了底层迭代器的种类的不同类型。因
此，如果 Iterator 是 input iterator 的话，我们的 projection iterator 也将是 input iterator，如
果 Iterator 是双向迭代器的话，我们的 projection iterator 也将是双向迭代器，以此类推。
Project()函数则使得 projection iterator 的构建变得很简单。
唯一缺少的是对 IteratorFacade 核心需求的实现。最有意思的是 dereference()，它会解引用
底层迭代器并投射到指向数据成员的指针：
T& dereference() const {
return (*iter).*member;
}
五车书馆
284
其余操作是依照底层迭代器实现的：
void increment() {
++iter;
}
bool equals(ProjectionIterator const& other) const {
return iter == other.iter;
}
void decrement() {
--iter;
}
为了简单起见，我们忽略了对随机访问迭代器的定义，但是其实现是类似的。
就这些！通过使用 projection iterator，我们可以打印出存储在 vector 中的 Person 数值的 first
name：
#include <vector>
#include <algorithm>
#include <iterator>
int main()
{
std::vector<Person> authors = { {"David", "Vandevoorde"},
{"Nicolai", "Josuttis"},
{"Douglas", "Gregor"} };
std::copy(project(authors.begin(), &Person::firstName),
project(authors.end(), &Person::firstName),
std::ostream_iterator<std::string>(std::cout, "\n"));
}
Facade 模式在创建需要符合特定接口的新类型时异常有用。新的类型只需要向 facade 暴露
出少量和核心操作，后续 facade 会通过结合使用 CRTP 和 Barton-Nackman 技术提供完整且
正确的 public 接口。
   */
  
  namespace case9 {
  
    struct Person {
      
      std::string firstName;
      std::string lastName;
      
      friend std::ostream& operator<<(std::ostream& strm, Person const& p) {
        return strm << p.lastName << ", " << p.firstName;
      }
      
    };
  
    template<typename Iterator, typename T>
    class ProjectionIterator : public case8::IteratorFacade<ProjectionIterator<Iterator, T>, T, typename
      std::iterator_traits<Iterator>::iterator_category, T&, typename
      std::iterator_traits<Iterator>::difference_type>
    {
      using Base = typename std::iterator_traits<Iterator>::value_type;
      using Distance = typename std::iterator_traits<Iterator>::difference_type;
      Iterator iter;
      T Base::* member;
      friend class IteratorFacadeAccess;
      //implement core iterator operations for IteratorFacade
    public:
      ProjectionIterator(Iterator iter, T Base::* member)
        : iter(iter), member(member) { }
    };
    
    template<typename Iterator, typename Base, typename T>
    auto project(Iterator iter, T Base::* member) { return ProjectionIterator<Iterator, T>(iter, member); }
  
    void test() {
    
    }
  
  }

  class Tmp {
  public:
  
  };

}


int
//main()
main_ch21_2()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch21_2::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch21_2::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch21_2::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch21_2::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch21_2::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}