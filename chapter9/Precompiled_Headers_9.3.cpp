//
// Created by yangfeng on 2022/7/29.
//

#include "../headers.h"

/*
预编译头文件
  即使不使用模板，C++的头文件也会大到需要很长时间进行编译。
   而模板的引入则进一步加剧了这一问题，
  程序员对这一问题的抱怨促使编译器供应商提供了一种叫做预编译头文件（PCH: precomplied header）的方案来降低编译时间。
   这一方案不在 C++标准要求之中，因此其具体实现方式由编译器供应商自行决定。
  
  虽然我们没有过多的讨论创建以及使用预编译头文件的方式（该部分内容需要参考那些提供了该特性的 C++编译系统的文档），
   但是适当的了解其运作机制总是有帮助的。
   
  当编译器编译一个文件的时候，它会从文件头开始编译并一直编译到结尾。
  当编译器处理文件中的符号（可能来自 include 文件）时，它会调整自己的状态，
   比如在符号表中添加新的条目，以方便随后的查找。
   在做这些事情的时候，编译器也可能会在目标文件中产生出一些代码。
  
  预编译头文件方案的实现基于这样一个事实：
  在组织代码的时候，很多文件都以相同的几行代码作为开始。
  
  为了便于讨论，假设那些将要被编译文件的前 N 行内容都相同。
  这样就可以单独编译这 N 行代码，并将编译完成后的状态保存在一个预编译头文件中（precompiled header）。
  接着所有以这 N 行代码开始的文件，在编译时都会重新载入这个被保存的状态，然后从第 N+1 行开始编译。
  
  在这里需要指出，重新载入被保存的前 N 行代码的预编译状态可能会比再次编译这 N 行代码要快很多很多倍。
   但是保存这个状态可能要比单次编译这 N行代码慢的多，编译时间可能延长 20%到 200%。
   
  因此利用预编译头文件提高编译速度的关键点是；
  让尽可能多的文件，以尽可能多的相同的代码作为开始。
  也就是说在实践中，文件要以相同的#include 指令（它们可能占用大量的编译时间）开始。
   因此如果#include 头文件的顺序相同的话，就会对提高编译性能很有帮助。
   
  但是对下面的文件：
    #include <vector>
    #include <list>
    …
  和
    #include <list>
    #include <vector>
    …
  预编译头文件不会起作用，因为它们的起始状态并不一致（顺序不一致）。
   一些程序员认为，即使可能会错过一个利用预编译头文件加速文件编译的机会，也应该多#include 一些可能用不到的头文件。
   这样做可以大大简化预编译头文件的使用方式。
   
  比如通常可以创建一个包含所有标准头文件的头文件，称之为 std.hpp：
    #include <iostream>
    #include <string>
    #include <vector>
    #include <deque>
    #include <list>
    …
  这个文件可以被预编译，其它所有用到标准库的文件都可以直接在文件开始处include 这个头文件：
    #include "std.hpp"
    …
    
  这个文件的编译会花费一些时间，但是如果内存足够的话，
    预编译方案的编译速度几乎要比在不使用预编译方案时编译其它任何一个标准库头文件都要快。
    
  标准头文件尤其适用于这一情况，因为它们很少发生变化，因此 std.hpp 的预编译头文件只会被编译一次。
  
  另外，预编译头文件也是项目依赖项配置的一部分（比如主流的 make 工具或者 IDE 工具在必要的时候会对它们进行更新）。
  
  一个值得推荐的组织预编译头文件的方法是将它们按层级组织，
    从最常用以及最稳定的头文件（比如 std.hpp）到那些我们期望其一直都不会变化的（因此值得被预编译的）头文件。
  但是如果头文件正处于频繁的开发阶段，为它们创建预编译头文件可能会增加编译时间，而不是减少编译时间。
  总之记住一点，为稳定层创建的预编译头文件可以被重复使用，以提高那些不太稳定的头文件的编译速度。
  
  比如，除了上面已经预编译过的 std.hpp 文件，还有一个专为我们的项目准备的、尚未达到稳定状态的头文件 core.hpp：
    #include "std.hpp"
    #include "core_data.hpp
    #include "core_algos.hpp"
    …
    
  由于这个文件（称之为 core.hpp）以#include “std.hpp”开始，编译器会去载入其对应的预编译头文件，
    然后继续编译之后的头文件，这期间不会对标准头文件进行再次编译。
    当整个文件处理完毕之后，就又产生了一个新的预编译头文件。
    由于编译器可以直接载入这个预编译的头文件，
     其它的应用就可以通过#include “core.hpp”头文件快速地使用其中的大量函数。
 */

using namespace std;

namespace ch9_3 {

  class Tmp {
  public:
  
  };

}

int
//main()
main_precompiled_headers()
{

  return 0;
}