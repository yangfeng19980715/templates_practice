//
// Created by yangfeng on 2022/9/9.
//

#include "../headers.h"

using namespace std;

/*
 24.2 类型列表的算法
基础的类型列表操作Front，PopFront和PushFront可以被组合起来实现更有意思的列表操作。
比如通过将 PushFront 作用于 PopFront 可以实现对第一个元素的替换：
using Type = PushFront<PopFront<SignedIntegralTypes>, bool>;
// equivalent to Typelist<bool, short, int, long, long long>
更近一步，我们可以按照模板原函数的实现方式，实现作用于类型列表的诸如搜索、转换和
反转等操作。
24.2.1 索引（Indexing）
类型列表的一个非常基础的操作是从列表中提取某个特定的类型。第 24.1 节展示了提取第
一个元素的实现方式。接下来我们将这一操作推广到可以提取第 Nth 个元素。比如，为了提
取给定类型列表中的第 2 个元素，可以这样：
using TL = NthElement<Typelist<short, int, long>, 2>;
这相当于将 TL 作为 long 的别名使用。NthElement 操作的实现方式是使用一个递归的元
程序遍历 typelist 中的元素，直到找到所需元素为止：
// recursive case:
template<typename List, unsigned N>
class NthElementT : public NthElementT<PopFront<List>, N-1>
{};
// basis case:
template<typename List>
class NthElementT<List, 0> : public FrontT<List>
{ };
template<typename List, unsigned N>
using NthElement = typename NthElementT<List, N>::Type;
首先来看由 N = 0 部分特例化出来的基本情况。这一特例化会通过返回类型列表中的第一个
元素来终止递归。其方法是对 FrontT<List>进行 public 继承，这样 FrontT<List>作为类型列表
中第一个元素的 Type 类型别名，就可以被作为 NthElement 的结果使用了（这里用到了元函
数转发，参见 19.3.2 节，但是译者没找到具体内容）。
作为模板主要部分的递归代码，会遍历类型列表。由于偏特化部分保证了 N > 0，递归部分
的代码会不断地从剩余列表中删除第一个元素并请求第 N-1 个元素。在我们的例子中：
NthElementT<Typelist<short, int, long>, 2>
继承自：
五车书馆
323
NthElementT<Typelist<int, long>, 1>
而它又继承自：
NthElementT<Typelist<long>, 0>
这 里 遇 到 了 最 基 本 的 N = 0 的 情 况 ， 它 继 承 自 提 供 了 最 终 结 果 Type 的
FrontT<Typelist<long>>。
24.2.2 寻找最佳匹配
有些类型列表算法会去查找类型列表中的数据。例如可能想要找出类型列表中最大的类型
（比如为了开辟一段可以存储类型列表中任意类型的内存）。这同样可以通过递归模板元程
序实现：
template<typename List>
class LargestTypeT;
// recursive case:
template<typename List>
class LargestTypeT
{
private:
using First = Front<List>;
using Rest = typename LargestTypeT<PopFront<List>>::Type;
public:
using Type = IfThenElse<(sizeof(First) >= sizeof(Rest)), First,
Rest>;
};
// basis case:
template<>
class LargestTypeT<Typelist<>>
{
public:
using Type = char;
};
template<typename List>
using LargestType = typename LargestTypeT<List>::Type;
LargestType 算法会返回类型列表中第一个最大的类型。比如对于 Typelist<bool, int,
long, short>，该算法会返回第一个大小和 long 相同的类型，可能是 int 也可能是 long，
取决于你的平台。
五车书馆
324
由于递归算法的使用，对 LargestTypeT 的调用次数会翻倍。它使用了 first/rest 的概念，
分三步完成任务。在第一步中，它先只基于第一个元素计算出部分结果，在本例中是将第一
个元素放置到 First 中。接下来递归地计算类型列表中剩余部分的结果，并将结果放置在 Rest
中。比如对于类型列表 Typelist<bool, int, long, short>，在递归的第一步中 First
是 bool，而 Rest 是该算法作用于 Typelist<int, long, short>得到的结果。最后在第
三步中综合 First 和 Rest 得到最终结果。此处，IfThenElse 会选出列表中第一个元素（First）
和到目前为止的最优解（Rest）中类型最大的那一个。>=的使用会倾向于选择第一个出现的
最大的类型。
递归会在类型列表为空时终结。默认情况下我们将 char 用作哨兵类型来初始化该算法，因
为任何类型都不会比 char 小。
注意上文中的基本情况显式的用到了空的类型列表 Typelist<>。这样有点不太好，因为它
可能会妨碍到其它类型的类型列表（我们会在第 24.3 节和第 24.5 节中讲到这一类类型列表）
的使用。为了解决这一问题，引入了 IsEmpty 元函数，它可以被用来判断一个类型列表是否
为空：
template<typename List>
class IsEmpty
{
public:
static constexpr bool value = false;
};
template<>
class IsEmpty<Typelist<>> {
public:
static constexpr bool value = true;
};
结合 IsEmpty，可以像下面这样将 LargestType 实现成适用于任意支持了 Front，PopFront 和
IsEmpty 的类型：
template<typename List, bool Empty = IsEmpty<List>::value>
class LargestTypeT;
// recursive case:
template<typename List>
class LargestTypeT<List, false>
{
private:
using Contender = Front<List>;
using Best = typename LargestTypeT<PopFront<List>>::Type;
public:
using Type = IfThenElse<(sizeof(Contender) >=
sizeof(Best)),Contender, Best>;
五车书馆
325
};
// basis case:
template<typename List>
class LargestTypeT<List, true>
{
public:
using Type = char;
};
template<typename List>
using LargestType = typename LargestTypeT<List>::Type;
默认的 LargestTypeT 的第二个模板参数 Empty 会检查一个类型列表是否为空。如果不为空，
就递归地继续在剩余的列表中查找。如果为空，就会终止递归并返回作为初始结果的 char。
24.2.3 向类型类表中追加元素
通过 PushFront 可以向类型列表的头部添加一个元素，并产生一个新的类型列表。除此之外
我们还希望能够像在程序运行期间操作 std::list 和 std::vector 那样，向列表的末尾追加一个
元素。对于我们的 Typelist 模板，为实现支持这一功能的 PushBack，只需要对 24.1 节中的
PushFront 做一点小的修改：
template<typename List, typename NewElement>
class PushBackT;
template<typename… Elements, typename NewElement>
class PushBackT<Typelist<Elements…>, NewElement>
{
public:
using Type = Typelist<Elements…, NewElement>;
};
template<typename List, typename NewElement>
using PushBack = typename PushBackT<List, NewElement>::Type;
不过和实现 LargestType 的算法一样，可以只用 Front，PushFront，PopFront 和 IsEmpty 等基
础操作实现一个更通用的 PushBack 算法：
template<typename List, typename NewElement, bool =
IsEmpty<List>::value>
class PushBackRecT;
// recursive case:
template<typename List, typename NewElement>
class PushBackRecT<List, NewElement, false>
五车书馆
326
{
using Head = Front<List>;
using Tail = PopFront<List>;
using NewTail = typename PushBackRecT<Tail, NewElement>::Type;
public:
using Type = PushFront<Head, NewTail>;
};
// basis case:
template<typename List, typename NewElement>
class PushBackRecT<List, NewElement, true>
{
public:
using Type = PushFront<List, NewElement>;
};
// generic push-back operation:
template<typename List, typename NewElement>
class PushBackT : public PushBackRecT<List, NewElement> { };
template<typename List, typename NewElement>
using PushBack = typename PushBackT<List, NewElement>::Type;
PushBackRecT 会自行管理递归。对于最基本的情况，用 PushFront 将 NewElement 添加到空
的类型列表中。递归部分的代码则要有意思的多：它首先将类型列表分成首元素（Head）
和一个包含了剩余元素的新的类型列表（Tail）。新元素则被追加到 Tail 的后面，这样递归
的进行下去，就会生成一个 NewTail。然后再次使用 PushFront 将 Head 添加到 NewTail 的头
部，生成最终的类型列表。
接下来以下面这个简单的例子为例展开递归的调用过程：
PushBackRecT<Typelist<short, int>, long>
在最外层的递归代码中，Head 会被解析成 short，Tail 则被解析生 Typelist<int>。然后递归到：
PushBackRecT<Typelist<int>, long>
其中 Head 会被解析成 int，Tail 则被解析成 Typelist<>。
然后继续递归计算：
PushBackRecT<Typelist<>, long>
这 会 触 发 最 基 本 的 情 况 并 返 回 PushFront<Typelist<>,
long> ， 其 结 果 是
Typelist<long>。然后返回上一层递归，将之前的 Head 添加到返回结果的头部：
PushFront<int, Typelist<long>>
五车书馆
327
它会返回 Typelist<int, long>。然后继续返回上一层递归，将最外层的 Head（short）
添加到返回结果的头部：
PushFront<short, Typelist<int, long>>
然后就得到了最终的结果：
Typelist<short, int, long>
通用版的 PushBackRecT 适用于任何类型的类型列表。和本节中之前实现的算法一样，计算
过程中它需要的模板实例的数量和类型列表的长度 N 成正比（如果类型列表的长度为 N，那
么 PushBackRecT 实例和 PushFrontT 实例的数目都是 N+1，FrontT 和 PopFront 实例的数量为
N）。由于模板实例化对于编译器而言是一个很复杂的过程，因此通过计算模板实例的数目，
可以大致估算出编译器编译某个元程序所需要的时间。
对于比较大的模板元程序，编译时间可能会是一个问题，因此有必要设法去降低算法所需要
的模板实例的数目。事实上，第一版 PushBack 的实现（用 Typelist 进行了部分特例化）只需
要固定数量的模板实例化，这使得它要比通用版本的实现（在编译期）更高效。而且，由于
它被描述成 PushBackT 的一种偏特化，在对一个 Typelist 执行 PushBack 的时候这一高效的实
现会被自动选择，从而为模板元程序引入了“算法特化”的概念（参见 20.1 节）。该章节
中介绍的很多技术都可以被模板元程序用来降低算法所需模板实例的数量。
24.2.4 类型列表的反转
当类型列表的元素之间有某种顺序的时候，对于某些算法而言，如果能够反转该顺序的话，
事情将会变得很方便。比如在 24.1 节介绍的 SignedIntegralTypes 中元素是按整型大小的等级
递增的。但是对其元素反转之后得到的 Typelist<long, long, long, int, short,
signed char>可能会更有用。下面的 Reverse 算法实现了相应的元函数：
template<typename List, bool Empty = IsEmpty<List>::value>
class ReverseT;
template<typename List>
using Reverse = typename ReverseT<List>::Type;
// recursive case:
template<typename List>
class ReverseT<List, false>:public PushBackT<Reverse<PopFront<List>>,
Front<List>> { };
// basis case:
template<typename List>
class ReverseT<List, true>{
public:
using Type = List;
};
五车书馆
328
该元函数的基本情况是一个作用于空的类型列表的函数。递归的情况则将类型列表分割成第
一个元素和剩余元素两部分。比如对于 Typelist<short, int, long>，递归过程会先
将第一个元素（short）从剩余元素（Typelist<int, long>）中分离开。然后递归得反转
列表中剩余的元素（生成 Typelist<long, int>），最后通过调用 PushBackT 将首元素追
加到被反转的列表的后面（生成 Typelist<long, int, short>）.
结合 Reverse，可以实现移除列表中最后一个元素的 PopBackT 操作：
template<typename List>
class PopBackT {
public:
u
sing Type = Reverse<PopFront<Reverse<List>>>;
};
template<typename List>
using PopBack = typename PopBackT<List>::Type;
该算法先反转整个列表，然后删除首元素并将剩余列表再次反转，从而实现删除末尾元素的
目的。
24.2.5 类型列表的转换
之前介绍的类型列表的相关算法允许我们从类型列表中提取任意元素，在类型列表中做查
找，构建新的列表以及反转列表。但是我们还需要对类型列表中的元素执行一些其它的操作。
比如可能希望对类型列表中的所有元素做某种转换，例如通过 AddConst 给列表中的元素加
上 const 修饰符：
template<typename T>
struct AddConstT
{
using Type = T const;
};
template<typename T>
using AddConst = typename AddConstT<T>::Type;
为了实现这一目的，相应的算法应该接受一个类型列表和一个元函数作为参数，并返回一个
将该元函数作用于类型列表中每个元素之后，得到的新的类型列表。比如：
Transform<SignedIntegralTypes, AddConstT>
返回的是一个包含了 signed char const, short const, int const, long const
和 long long
const 的类型列表。元函数被以模板参数模板（参见 5.7 节）的形式提供，
它负责将一种类型转换为另一种类型。Transform 算法本身和预期的一样是一个递归算法：
template<typename List, template<typename T> class MetaFun, bool Empty
五车书馆
329
= IsEmpty<List>::value>
class TransformT;
// recursive case:
template<typename List, template<typename T> class MetaFun>
class TransformT<List, MetaFun, false>
: public PushFrontT<typename TransformT<PopFront<List>,
MetaFun>::Type, typename MetaFun<Front<List>>::Type>
{};
// basis case:
template<typename List, template<typename T> class MetaFun>
class TransformT<List, MetaFun, true>
{
public:
using Type = List;
};
template<typename List, template<typename T> class MetaFun>
using Transform = typename TransformT<List, MetaFun>::Type;
此处的递归情况虽然句法比较繁琐，但是依然很直观。最终转换的结果是第一个元素的转换
结果，加上对剩余元素执行执行递归转换后的结果。
在第 24.4 节介绍了一种更为高效的 Transform 的实现方法。
24.2.6 类型列表的累加（Accumulating Typelists）
转换（Transform）算法在需要对类型列表中的元素做转换时很有帮助。通常将它和累加
（Accumulate）算法一起使用，它会将类型列表中的所有元素组合成一个值。Accumulate 算
法以一个包含元素 T1，T2，...，TN 的类型列表 T，一个初始类型 I，和一个接受两个类型作
为参数的元函数 F 为参数，并最终返回一个类型。它的返回值是 F (F (F (…F(I, T1),
T2), …, TN−1), TN )，其中在第 ith 步，F 将作用于前 i-1 步的结果以及 Ti。
取决于具体的类型列表，F 的选择以及初始值 I 的选择，可以通过 Accumulate 产生各种不
同的输出。比如如果 F 可以被用来在两种类型中选择较大的那一个，Accumulate 的行为就和
LargestType 差不多。而如果 F 接受一个类型列表和一个类型作为参数，并且将类型追加到类
型列表的后面，其行为又和 Reverse 算法差不多。
Accumulate 的实现方式遵循了标准的递归元编程模式：
template<typename List,
template<typename X, typename Y> class F,
typename I,
五车书馆
330
bool = IsEmpty<List>::value>
class AccumulateT;
// recursive case:
template<typename List,
template<typename X, typename Y> class F,
typename I>
class AccumulateT<List, F, I, false>
: public AccumulateT<PopFront<List>, F,
typename F<I, Front<List>>::Type>
{};
// basis case:
template<typename List,
template<typename X, typename Y> class F,
typename I>
class AccumulateT<List, F, I, true>
{
public:
using Type = I;
};
template<typename List,
template<typename X, typename Y> class F,
typename I>
using Accumulate = typename AccumulateT<List, F, I>::Type;
这里初始类型 I 也被当作累加器使用，被用来捕捉当前的结果。因此当递归到类型列表末
尾的时候，递归循环的基本情况会返回这个结果。在递归情况下，算法将 F 作用于之前的结
果（I）以及当前类型列表的首元素，并将 F 的结果作为初始类型继续传递，用于下一级对
剩余列表的求和（Accumulating）。
有了 Accumulate，就可以通过将 PushFrontT 作为元函数 F，将空的类型列表（TypeList<T>）
作为初始类型 I，反转一个类型列表：
using Result = Accumulate<SignedIntegralTypes, PushFrontT,
Typelist<>>;
// produces TypeList<long long, long, int, short, signed char>
如果要实现基于 Accumulate 的 LargestType（称之为 LargestTypeAcc），还需要做一些额外的
工作，因为首先要实现一个返回两种类型中类型较大的那一个的元函数：
template<typename T, typename U>
class LargerTypeT
: public IfThenElseT<sizeof(T) >= sizeof(U), T, U>
{ };
五车书馆
331
template<typename Typelist>
class LargestTypeAccT
: public AccumulateT<PopFront<Typelist>, LargerTypeT,
Front<Typelist>>
{ };
template<typename Typelist>
using LargestTypeAcc = typename LargestTypeAccT<Typelist>::Type;
值得注意的是，由于这一版的 LargestType 将类型列表的第一个元素当作初始类型，因此其
输入不能为空。我们可以显式地处理空列表的情况，要么是返回一个哨兵类型（char 或者
void），要么让该算法很好的支持 SFINASE，就如同 19.4.4 节讨论的那样：
template<typename T, typename U>
class LargerTypeT
: public IfThenElseT<sizeof(T) >= sizeof(U), T, U>
{ };
template<typename Typelist, bool = IsEmpty<Typelist>::value>
class LargestTypeAccT;
template<typename Typelist>
class LargestTypeAccT<Typelist, false>
: public AccumulateT<PopFront<Typelist>, LargerTypeT,
Front<Typelist>>
{ };
template<typename Typelist>
class LargestTypeAccT<Typelist, true>
{ };
template<typename Typelist>
using LargestTypeAcc = typename LargestTypeAccT<Typelist>::Type;
Accumulate 是一个非常强大的类型列表算法，利用它可以实现很多种操作，因此可以将其看
作类型列表操作相关的基础算法。
24.2.7 插入排序
作为最后一个类型列表相关的算法，我们来介绍插入排序。和其它算法类似，其递归过程会
将类型列表分成第一个元素（Head）和剩余的元素（Tail）。然后对 Tail 进行递归排序，并
将 Head 插入到排序后的类型列表中的合适的位置。该算法的实现如下：
template<typename List, template<typename T, typename U>
class Compare,
五车书馆
332
bool = IsEmpty<List>::value>
class InsertionSortT;
template<typename List, template<typename T, typename U> class Compare>
using InsertionSort = typename InsertionSortT<List, Compare>::Type;
// recursive case (insert first element into sorted list):
template<typename List, template<typename T, typename U> class Compare>
class InsertionSortT<List, Compare, false>
: public InsertSortedT<InsertionSort<PopFront<List>, Compare>,
Front<List>, Compare>
{};
// basis case (an empty list is sorted):
template<typename List, template<typename T, typename U> class Compare>
class InsertionSortT<List, Compare, true>
{
public:
using Type = List;
};
在对类型列表进行排序时，参数 Compare 被用来作比较。它接受两个参数并通过其 value 成
员返回一个布尔值。将其用来处理空列表的情况会稍嫌繁琐。
插入排序算法的核心时元函数 InsertSortedT，它将一个值插入到一个已经排序的列表中（插
入到第一个可能的位置）并保持列表依然有序：
#include "identity.hpp"
template<typename List, typename Element,
template<typename T, typename U> class Compare, bool =
IsEmpty<List>::value>
class InsertSortedT;
// recursive case:
template<typename List, typename Element, template<typename T,
typename U> class Compare>
class InsertSortedT<List, Element, Compare, false>
{
// compute the tail of the resulting list:
using NewTail = typename IfThenElse<Compare<Element,
Front<List>>::value, IdentityT<List>,
InsertSortedT<PopFront<List>,
Element, Compare>>::Type;
// compute the head of the resulting list:
using NewHead = IfThenElse<Compare<Element, Front<List>>::value,
五车书馆
333
Element, Front<List>>;
public:
using Type = PushFront<NewTail, NewHead>;
};
// basis case:
template<typename List, typename Element, template<typename T,
typename U> class Compare>
class InsertSortedT<List, Element, Compare, true>
: public PushFrontT<List, Element>
{};
template<typename List, typename Element,template<typename T, typename
U> class Compare>
using
InsertSorted
=
typename
InsertSortedT<List,
Element,
Compare>::Type;
由于只有一个元素的列表是已经排好序的，因此相关代码不是很复杂。对于递归情况，基于
元素应该被插入到列表头部还是剩余部分，其实现也有所不同。如果元素应该被插入到（已
经排序的）列表第一个元素的前面，那么就用 PushFront 直接插入。否则，就将列表分成 head
和 tail 两部分，这样递归的尝试将元素插入到 tail 中，成功之后再用 PushFront 将 head 插入
到 tail 的前面。
上述实现中包含了一个避免去实例化不会用到的类型的编译期优化，在第 19.7.1 节对该技术
（去看看）进行了讨论。下面这个实现在技术上也是正确的：
template<typename List, typename Element, template<typename T,
typename U> class Compare>
class InsertSortedT<List, Element, Compare, false>
: public IfThenElseT<Compare<Element, Front<List>>::value,
PushFront<List, Element>,
PushFront<InsertSorted<PopFront<List>, Element,
Compare>, Front<List>>>
{};
但是由于这种递归情况的实现方式会计算 IfThenElseT 的两个分支（虽然只会用到一个），
其效率会受到影响。在这个实现中，在 IfThenElseT 的 then 分支中使用 PushFront 的成本非
常低，但是在 else 分支中递归地使用 InsertSorted 的成本则很高。
在我们的优化实现中，第一个 IfThenElse 会计算出列表的 tail（NewTail）。其第二和第三个
参数是用来计算特定结果的元函数。Then 分支中的参数使用 IdentityT（参见 19.7.1 节）来
计算未被修改的 List。Else 分支中的参数用 InsertSortedT 来计算将元素插入到已排序列表之
后的结果。在较高层面上，Identity 和 InsertSortedT 两者中只有一个会被实例化，因此不会
有太多的额外工作。
五车书馆
334
第二个 IfThenElse 会计算上面获得的 list 的 head，其两个分支的计算代价都很低，因此都会
被立即计算。最终的结果由 NewHead 和 NewTail 计算得到。
这一实现方案所需要的实例化数目，与被插入元素在一个已排序列表中的插入位置成正比。
这表现为更高级别的插入排序属性：排序一个已经有序的列表，所需要实例化的数目和列表
的长度成正比（如果已排序列表的排列顺序和预期顺序相反的话，所需要的实例化数目和列
表长度的平方成正比）。
下面的程序会基于列表中元素的大小，用插入排序对其排序。比较函数使用了 sizeof 运算符
并比较其结果：
template<typename T, typename U>
struct SmallerThanT {
static constexpr bool value = sizeof(T) < sizeof(U);
};
void testInsertionSort()
{
using Types = Typelist<int, char, short, double>;
using ST = InsertionSort<Types, SmallerThanT>;
std::cout << std::is_same<ST,Typelist<char, short, int,
double>>::value << ’\n’;
}
 */

namespace ch24_2 {
  /*
   24.2 类型列表的算法
基础的类型列表操作Front，PopFront和PushFront可以被组合起来实现更有意思的列表操作。
比如通过将 PushFront 作用于 PopFront 可以实现对第一个元素的替换：
using Type = PushFront<PopFront<SignedIntegralTypes>, bool>;
// equivalent to Typelist<bool, short, int, long, long long>
更近一步，我们可以按照模板原函数的实现方式，实现作用于类型列表的诸如搜索、转换和
反转等操作。
   */
  
  namespace case1 {
    
    template<typename... Elements>
    class Typelist {
    };
    
    using SignedIntegralTypes = Typelist<signed char, short, int, long, long long>;
    
    template<typename List>
    class FrontT;
    
    template<typename Head, typename... Tail>
    class FrontT<Typelist<Head, Tail...>> {  // 注意这里的特化方式，如果不用这个特化，就无法使用template<typename List> using Front = typename FrontT<List>::Type;
    public:
      using Type = Head;
    };
    
    template<typename List>
    using Front = typename FrontT<List>::Type;  // 基于List，推断出FrontT中的模板参数head和tail
    
    template<typename List>
    class PopFrontT;
    
    template<typename Head, typename... Tail>
    class PopFrontT<Typelist<Head, Tail...>> {
    public:
      using Type = Typelist<Tail...>;
    };
    
    template<typename List>
    using PopFront = typename PopFrontT<List>::Type;
    
    template<typename List, typename NewElement>
    class PushFrontT;
    
    template<typename... Elements, typename NewElement>
    class PushFrontT<Typelist<Elements...>, NewElement> {
    public:
      using Type = Typelist<NewElement, Elements...>;
    };
    
    template<typename List, typename NewElement>
    using PushFront = typename PushFrontT<List, NewElement>::Type;
    
    // recursive case:
    template<typename List, unsigned N>  // 这里的继承会一路展开，直到 N-1 == 0
    class NthElementT : public NthElementT<PopFront<List>, N - 1> {
    };
    
    // basis case:
    template<typename List>
    class NthElementT<List, 0> : public FrontT<List> {
    };
    
    template<typename List, unsigned N>
    using NthElement = typename NthElementT<List, N>::Type;
    
    /*
     24.2.2 寻找最佳匹配
有些类型列表算法会去查找类型列表中的数据。例如可能想要找出类型列表中最大的类型
（比如为了开辟一段可以存储类型列表中任意类型的内存）。这同样可以通过递归模板元程
序实现：
     */
    
    template<typename List>
    class LargestTypeT_old;
    
    // recursive case:
    template<typename List>
    class LargestTypeT_old {
    private:
      using First = Front<List>;
      using Rest = typename LargestTypeT_old<PopFront<List>>::Type;
    
    public:
      using Type = std::conditional<(sizeof(First) >= sizeof(Rest)), First, Rest>;
    };
    
    // basis case:
    template<>
    class LargestTypeT_old<Typelist<>> {
    public:
      using Type = char;
    };
    
    template<typename List>
    using LargestType_old = typename LargestTypeT_old<List>::Type;
    
    /*
    注意上文中的基本情况显式的用到了空的类型列表 Typelist<>。这样有点不太好，因为它
      可能会妨碍到其它类型的类型列表（我们会在第 24.3 节和第 24.5 节中讲到这一类类型列表）
    的使用。为了解决这一问题，引入了 IsEmpty 元函数，它可以被用来判断一个类型列表是否为空：
      */
    template<typename List>
    class IsEmpty {
    public:
      static constexpr bool value = false;
    };
    
    template<>
    class IsEmpty<Typelist<>> {
    public:
      static constexpr bool value = true;
    };
    
    // 结合 IsEmpty，可以像下面这样将 LargestType 实现成适用于任意支持了 Front，PopFront 和IsEmpty 的类型：
    template<typename List, bool Empty = IsEmpty<List>::value>
    class LargestTypeT;
    
    
    // recursive case:
    template<typename List>
    class LargestTypeT<List, false> {
    private:
      using Contender = Front<List>;
      using Best = typename LargestTypeT<PopFront<List>>::Type;
    
    public:
      using Type = std::conditional<(sizeof(Contender) >= sizeof(Best)), Contender, Best>;
    };
    
    // basis case:
    template<typename List>
    class LargestTypeT<List, true> {
    public:
      using Type = char;
    };
    
    template<typename List>
    using LargestType = typename LargestTypeT<List>::Type;
    
    /*
    默认的 LargestTypeT 的第二个模板参数 Empty 会检查一个类型列表是否为空。如果不为空，
    就递归地继续在剩余的列表中查找。如果为空，就会终止递归并返回作为初始结果的 char。
     */
    
    /*
    24.2.3 向类型类表中追加元素
      通过 PushFront 可以向类型列表的头部添加一个元素，并产生一个新的类型列表。除此之外
      我们还希望能够像在程序运行期间操作 std::list 和 std::vector 那样，向列表的末尾追加一个
      元素。对于我们的 Typelist 模板，为实现支持这一功能的 PushBack，只需要对 24.1 节中的
      PushFront 做一点小的修改：
      */
    template<typename List, typename NewElement>
    class PushBackT;
    
    template<typename... Elements, typename NewElement>
    class PushBackT<Typelist<Elements...>, NewElement> {
    public:
      using Type = Typelist<Elements..., NewElement>;
    };
    
    template<typename List, typename NewElement>
    using PushBack = typename PushBackT<List, NewElement>::Type;
    
    // 不过和实现 LargestType 的算法一样，
    //     可以只用 Front，PushFront，PopFront 和 IsEmpty 等基础操作实现一个更通用的 PushBack 算法：
    template<typename List, typename NewElement, bool = IsEmpty<List>::value>
    class PushBackRecT;
    
    // recursive case:
    template<typename List, typename NewElement>
    class PushBackRecT<List, NewElement, false> {
    private:
      using Head = Front<List>;
      using Tail = PopFront<List>;
      using NewTail = typename PushBackRecT<Tail, NewElement>::Type;
      
    public:
      using Type = PushFront<Head, NewTail>;
    };
    
    // basis case:
    template<typename List, typename NewElement>
    class PushBackRecT<List, NewElement, true> {
    public:
      using Type = PushFront<List, NewElement>;
    };
    
    // generic push-back operation:
    template<typename List, typename NewElement>
    class PushBackT : public PushBackRecT<List, NewElement> { };
  
    template<typename List, typename NewElement>
    using PushBack = typename PushBackT<List, NewElement>::Type;
  
    /*
      24.2.4 类型列表的反转
当类型列表的元素之间有某种顺序的时候，对于某些算法而言，如果能够反转该顺序的话，
事情将会变得很方便。比如在 24.1 节介绍的 SignedIntegralTypes 中元素是按整型大小的等级
递增的。但是对其元素反转之后得到的 Typelist<long, long, long, int, short,
signed char>可能会更有用。下面的 Reverse 算法实现了相应的元函数：
     */
    template<typename List, bool Empty = IsEmpty<List>::value>
    class ReverseT;
  
    template<typename List>
    using Reverse = typename ReverseT<List>::Type;
  
    // recursive case:
    template<typename List>
    class ReverseT<List, false> : public PushBackT<Reverse<PopFront<List>>, Front<List>> { };
  
    // basis case:
    template<typename List>
    class ReverseT<List, true> {
    public:
      using Type = List;
    };
  
    // 结合 Reverse，可以实现移除列表中最后一个元素的 PopBackT 操作：
    template<typename List>
    class PopBackT {
    public:
      using Type = Reverse<PopFront<Reverse<List>>>;
    };
    
    template<typename List>
    using PopBack = typename PopBackT<List>::Type;
    
    /*
     24.2.5 类型列表的转换
之前介绍的类型列表的相关算法允许我们从类型列表中提取任意元素，在类型列表中做查
找，构建新的列表以及反转列表。但是我们还需要对类型列表中的元素执行一些其它的操作。
比如可能希望对类型列表中的所有元素做某种转换，例如通过 AddConst 给列表中的元素加
上 const 修饰符：
     */

    template<typename T>
    struct AddConstT {
      using Type = T const;
    };
    
    template<typename T>
    using AddConst = typename AddConstT<T>::Type;
    
    /*
     为了实现这一目的，相应的算法应该接受一个类型列表和一个元函数作为参数，并返回一个
      将该元函数作用于类型列表中每个元素之后，得到的新的类型列表。比如：Transform<SignedIntegralTypes, AddConstT>
     
     返回的是一个包含了 signed char const, short const, int const, long const和 long long const 的类型列表。
     元函数被以模板参数模板（参见 5.7 节）的形式提供，它负责将一种类型转换为另一种类型。Transform 算法本身和预期的一样是一个递归算法：
     */
    template< typename List,
              template<typename T> class MetaFun,
              bool Empty = IsEmpty<List>::value
            >
    class TransformT;
    
    // recursive case:
    template<typename List, template<typename T> class MetaFun>
    class TransformT<List, MetaFun, false> :
      public PushFrontT<typename TransformT<PopFront<List>, MetaFun>::Type,
                        typename MetaFun<Front<List>>::Type
                        > {};
    
    // basis case:
    template<typename List, template<typename T> class MetaFun>
    class TransformT<List, MetaFun, true> {
    public:
      using Type = List;
    };
    
    template<typename List, template<typename T> class MetaFun>
    using Transform = typename TransformT<List, MetaFun>::Type;
    
    /*
      此处的递归情况虽然句法比较繁琐，但是依然很直观。最终转换的结果是第一个元素的转换结果，加上对剩余元素执行执行递归转换后的结果。
     */
    
    /*
     24.2.6 类型列表的累加（Accumulating Typelists）
转换（Transform）算法在需要对类型列表中的元素做转换时很有帮助。通常将它和累加
（Accumulate）算法一起使用，它会将类型列表中的所有元素组合成一个值。Accumulate 算
法以一个包含元素 T1，T2，...，TN 的类型列表 T，一个初始类型 I，和一个接受两个类型作
为参数的元函数 F 为参数，并最终返回一个类型。它的返回值是 F (F (F (…F(I, T1),
T2), …, TN−1), TN )，其中在第 ith 步，F 将作用于前 i-1 步的结果以及 Ti。
取决于具体的类型列表，F 的选择以及初始值 I 的选择，可以通过 Accumulate 产生各种不
同的输出。比如如果 F 可以被用来在两种类型中选择较大的那一个，Accumulate 的行为就和
LargestType 差不多。而如果 F 接受一个类型列表和一个类型作为参数，并且将类型追加到类
型列表的后面，其行为又和 Reverse 算法差不多。
Accumulate 的实现方式遵循了标准的递归元编程模式：
     */
    template<typename List,                             // 类型列表 T
             template<typename X, typename Y> class F,  // 元函数 F
             typename I,                                // 初始类型 I
             bool = IsEmpty<List>::value
            >
    class AccumulateT;
    
    // recursive case:
    template<typename List,
            template<typename X, typename Y> class F,
            typename I
            >
    class AccumulateT<List, F, I, false>
      : public AccumulateT<PopFront<List>, F, typename F<I, Front<List>>::Type> {};
    
    // basis case:
    template< typename List,
              template<typename X, typename Y> class F,
              typename I
            >
    class AccumulateT<List, F, I, true> {
    public:
      using Type = I;
    };
    
    template< typename List,
              template<typename X, typename Y> class F,
              typename I
            >
    using Accumulate = typename AccumulateT<List, F, I>::Type;
  
    // 有了 Accumulate，就可以通过将 PushFrontT 作为元函数 F，将空的类型列表（TypeList<T>） 作为初始类型 I，反转一个类型列表：
    using Result = Accumulate<SignedIntegralTypes, PushFrontT, Typelist<>>;      // produces TypeList<long long, long, int, short, signed char>
  
    // 如果要实现基于 Accumulate 的 LargestType（称之为 LargestTypeAcc），还需要做一些额外的工作，因为首先要实现一个返回两种类型中类型较大的那一个的元函数：
    
    template <bool cond, typename T, typename U>
    using IfThenElseT = std::conditional<cond, T, U>;
    
    template<typename T, typename U>
    class LargerTypeT : public IfThenElseT<sizeof(T) >= sizeof(U), T, U> { };
    
    template<typename Typelist>
    class LargestTypeAccT_old : public AccumulateT<PopFront<Typelist>, LargerTypeT, Front<Typelist>> { };
    
    template<typename Typelist>
    using LargestTypeAcc_old = typename LargestTypeAccT_old<Typelist>::Type;
    
    /*
    值得注意的是，由于这一版的 LargestType 将类型列表的第一个元素当作初始类型，因此其输入不能为空。
     我们可以显式地处理空列表的情况，要么是返回一个哨兵类型（char 或者 void），要么让该算法很好的支持 SFINASE，就如同 19.4.4 节讨论的那样：
     */
    
    template<typename Typelist, bool = IsEmpty<Typelist>::value>
    class LargestTypeAccT;
    
    template<typename Typelist>
    class LargestTypeAccT<Typelist, false> : public AccumulateT<PopFront<Typelist>, LargerTypeT, Front<Typelist>> { };
    
    template<typename Typelist>
    class LargestTypeAccT<Typelist, true> { };
    
    template<typename Typelist>
    using LargestTypeAcc = typename LargestTypeAccT<Typelist>::Type;
    // Accumulate 是一个非常强大的类型列表算法，利用它可以实现很多种操作，因此可以将其看作类型列表操作相关的基础算法。
  
    /*
    24.2.7 插入排序
      作为最后一个类型列表相关的算法，我们来介绍插入排序。和其它算法类似，其递归过程会
      将类型列表分成第一个元素（Head）和剩余的元素（Tail）。然后对 Tail 进行递归排序，并
      将 Head 插入到排序后的类型列表中的合适的位置。该算法的实现如下：
      */
    template< typename List,
              template<typename T, typename U> class Compare,
              bool = IsEmpty<List>::value
            >
    class InsertionSortT;
    
    template< typename List,
              template<typename T, typename U> class Compare
            >
    using InsertionSort = typename InsertionSortT<List, Compare>::Type;
    
    
    /*
      在对类型列表进行排序时，参数 Compare 被用来作比较。它接受两个参数并通过其 value 成员返回一个布尔值。
      将其用来处理空列表的情况会稍嫌繁琐。
      
      插入排序算法的核心时元函数 InsertSortedT，它将一个值插入到一个已经排序的列表中（插入到第一个可能的位置）并保持列表依然有序：
     */
    template< typename List,
              typename Element,
              template<typename T, typename U> class Compare,
              bool = IsEmpty<List>::value>
    class InsertSortedT;
    
    /*
    // recursive case:
    template< typename List,
              typename Element,
              template<typename T, typename U> class Compare>
    class InsertSortedT<List, Element, Compare, false> {
      // compute the tail of the resulting list:
      using NewTail = typename std::conditional<Compare<Element, Front<List>>::value, IdentityT<List>, InsertSortedT<PopFront<List>, Element, Compare>>::Type;
      
      // compute the head of the resulting list:
      using NewHead = std::conditional<Compare<Element, Front<List>>::value, Element, Front<List>>;
      
    public:
      using Type = PushFront<NewTail, NewHead>;
    };
    
    // basis case:
    template< typename List,
              typename Element,
              template<typename T, typename U> class Compare>
    class InsertSortedT<List, Element, Compare, true> : public PushFrontT<List, Element> {};
    
    template<typename List,
             typename Element,
             template<typename T, typename U> class Compare>
    using InsertSorted = typename InsertSortedT<List, Element, Compare>::Type;
  
    template<typename T, typename U>
    struct SmallerThanT {
      static constexpr bool value = sizeof(T) < sizeof(U);
    };
    
    // recursive case (insert first element into sorted list):
    template<typename List, template<typename T, typename U> class Compare>
    class InsertionSortT<List, Compare, false>
      : public InsertSortedT<InsertionSort<PopFront<List>, Compare>, Front<List>, Compare> {};
    
    // basis case (an empty list is sorted):
    template<typename List, template<typename T, typename U> class Compare>
    class InsertionSortT<List, Compare, true> {
    public:
      using Type = List;
    };
    
    void testInsertionSort()
    {
      using Types = Typelist<int, char, short, double>;
      using ST = InsertionSort<Types, SmallerThanT>;
      std::cout << std::is_same<ST,Typelist<char, short, int, double>>::value << std::endl;
    }
     */
    
    
    void test() {
      cout << "hello, world" << endl;
    }
    
  }
  
  namespace case2 {
    
    void test() {
    
    }
    
  }
  
  namespace case3 {
    
    void test() {
    
    }
    
  }
  
  namespace case4 {
    
    void test() {
    
    }
    
  }
  
  namespace case5 {
    
    void test() {
    
    }
    
  }
  
  class Tmp {
  public:
  
  };
  
}

int
//main()
main_ch24_2()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch24_2::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch24_2::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch24_2::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch24_2::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch24_2::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;
  
  return 0;
}