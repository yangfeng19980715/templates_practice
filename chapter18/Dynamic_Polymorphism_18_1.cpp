//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

/*
多态是一种用单个统一的符号将多种特定行为关联起来的能力。
 多态也是面向对象编程范式的基石，在 C++中它主要由继承和虚函数实现。
 由于这一机制主要（至少是一部分）在运行期间起作用，因此我们称之为动态多态（dynamic polymorphism）。
 它也是我们通常在讨论C++中的简单多态时所指的多态。
 但是，模板也允许我们用单个统一符号将不同的特定行为关联起来，不过该关联主要发生在编译期间，我们称之为静态多态（static polymorphism）。
 
 在本章中我们将探讨这两种形式的多态，并讨论其各自所适用的情况。
 */

using namespace std;

/*
由于历史原因，C++在最开始的时候只支持通过继承和虚函数实现的多态。
 在此情况下，多态设计的艺术性主要体现在从一些相关的对象类型中提炼出一组统一的功能，
 然后将它们声明成一个基类的虚函数接口。
 
这一设计方式的范例之一是一种用来维护多种几何形状、并通过某些方式将其渲染的应用。

在这样一种应用中，我们可以发现一个抽线基类（abstract base class，ABC），
   在其中声明了适用于几何对象的统一的操作和属性。
   其余适用于特定几何对象的类都从它做了继承。
 */

namespace ch18_1 {

  namespace case1 {
  }
  
  namespace case2 {
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
main_Dynamic_Polymorphism_18_1()
{

  return 0;
}