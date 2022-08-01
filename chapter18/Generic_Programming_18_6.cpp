//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

/*
 Static 多态的出现引入了泛型编程的概念。
 但是，到目前为止并没有一个世所公认的泛型编程的定义（就如同也没有一个世所公认的面向对象编程的定义一样）。
 根据[CzarneckiEiseneckerGenProg] ，定义从针对泛型参数编程（ programming with generic parameters）
   发展到发现有效算法的最佳抽象表达（finding the most abstract representation of efficient algorithms）。
   
   概述总结到：
      泛型编程是计算机科学的一个分支，它主要处理的问题是寻找高效算法，数据结构，
       以及其它一些软件概念的抽象表达（结合它们自身的系统组织）。泛型编程专注于域概念族的表达。
       
    在 C++的语境中，泛型编程有时候也被定义成模板编程（而面向对象编程被认为是基于虚函数的编程）。
    在这个意义上，几乎任何 C++模板的使用都可以被看作泛型编程的实例。
    
    但是，开发者通常认为泛型编程还应包含如下这一额外的要素：
      该模板必须被定义于一个框架中，且必须能够适用于大量的、有用的组合。
    
    到目前为止，在该领域中最重要的一个贡献是标准模板库（Standard Template Library, STL），
       随后它也被调整并合并进了 C++标准库（C++ standard library）。
       STL 是一个框架，它提供了许多有用的操作（称为算法），用于对象集合（称为容器）的一些线性数据结构。
       算法和容器都是模板。
       但是，关键的是算法本身并不是容器的成员函数。算法被以一种泛型的方式实现，因此它们可以用于任意的容器类型（以及线性的元素集合）。
       
    为了实现这一目的，STL 的设计者们找到了一种可以用于任意线性集合、称之为迭代器（iterators）抽象概念。
    从本质上来说，容器操作中针对于集合的某些方面已经被分解到迭代器的功能中。
    这样，我们就可以在不知道元素的具体存储方式的情况下，实现一种求取序列中元素最大值的方法：
        template<typename Iterator>
        Iterator max_element (Iterator beg,                         //refers to start of collection
                            Iterator end)                           //refers to end of collection
        {
            // use only certain Iterator operations to traverse all elements
            // of the collection to find the element with the maximum value
            // and return its position as Iterator
        }
        
    这样就可以不用去给所有的线性容器都提供一些诸如 max_element（）的操作，
       容器本身只要提供一个能够遍历序列中数值的迭代器类型，以及一些能够创建这些迭代器的成员函数就可以了：
        namespace std {
            template<typename T, …>
            class vector {
            public:
                using const_iterator = …;                           // implementation-specific iterator
                … // type for constantvectors
                const_iterator begin() const;                       // iterator for start of collection
                const_iterator end() const;                         // iterator for end of collection
            };
            
            template<typename T, …>
            class list {
            public:
                using const_iterator = …;                           // implementation-specific iterator
                …// type for constant lists
                const_iterator begin() const;                       // iterator for start of collection
                const_iterator end() const;                         // iterator for end of collection
            };
        }
        
    现在就可以通过调用泛型操作 max_element()（以容器的 beginning 和 end 伟参数）来寻找任意集合中的最大值了（省略了对空集合的处理）：
        #include <vector>
        #include <list>
        #include <algorithm>
        #include <iostream>
        #include "MyClass.hpp"
        
        template<typename T>
        void printMax (T const& coll){
        
            // compute position of maximum value
            auto pos = std::max_element(coll.begin(),coll.end());
            
            // print value of maximum element of coll (if any):
            if (pos != coll.end()) {
                std::cout << *pos << ’\n’;
            }
            else {
                std::cout << "empty" << ’\n’;
            }
        }
        
        int main()
        {
            std::vector<MyClass> c1;
            std::list<MyClass> c2;
            …
            printMax(c1);
            printMax(c2);
        }
        
    通过用这些迭代器来参数化其操作，STL 避免了相关操作在定义上的爆炸式增长。
    我们并没有为每一种容器都把每一种操作定义一遍，而是只为一个算法进行一次定义，然后将其用于所有的容器。
    
    泛型的关键是迭代器，它由容器提供并被算法使用。
    这样之所以可行，是因为迭代器提供了特定的、可以被算法使用的接口。
    这些接口通常被称为 concept，它代表了为了融入该框架，模板必须满足的一组限制条件。
    此外，该概念还可用于其它一些操作和数据结构。
    
    你可能会想起我们曾在第 18.4 节介绍过一个叫做 concept 的语言特性（更多的细节可以参见附录 E），
        而事实上，该语言特性刚好和这里的概念对应。
        
    实际上在当前上下文中，concept 这个名词最早是由 STL 的设计者为了规范化它们的代码而引入的概念。
    在那之后，我们开始努力使这些概念在我们的模板中明确化。
    
    接下来的语言特性将帮助我们指出（以及检查）对迭代器的要求（
        因为有很多种迭代器类型，比如 forward 和 bidirectional 迭代器，因此也就会引入多种对应的 concept，参见 E.3.1）。
        
        不过在当今的 C++中，在泛型库（尤其是 C++标准库）的规格中这些 concept 通常都是隐式的。
        
    幸运的是，确实有一些特性和技术（比如 static_assert 和 SFINAE）允许我们进行一定数量的自动化检查。
    
    原则上，类似于 STL 方法的一类功能都可以用动态多态实现。
    但是在实际中，由于迭代器的 concept 相比于虚函数的调用过于轻量级，因此多态这一方法的用途有限。
    基于虚函数添加一个接口层，很可能会将我们的操作性能降低一个数量级（甚至更多）。
    
    泛型编程之所以实用，正是因为它依赖于静态多态，这样就可以在编译期间就决定具体的接口。
    另一方面，需要在编译期间解析出接口的这一要求，又催生出了一些与面向对象设计原则（object oriented principles）不同的新原则。
    这些泛型设计原则（generic design principles）中最重要的一部分将会在本书剩余的章节中介绍。
    
    另外，附录 E 通过描述对 concept 概念的直接语言支持，将泛型编程当作一种开发范式进行了深入分析。
 */

using namespace std;

namespace ch18_6 {

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
main_Generic_Programming_18_6()
{

    return 0;
}