//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

using namespace std;

/*
使用 concepts
    针对使用了模板的静态多态的一个争议是，接口的绑定是通过实例化相应的模板执行的。
    也就是说没有可供编程的公共接口或者公共 class。
    取而代之的是，如果所有实例化的代码都是有效的，那么对模板的任何使用也都是有效的。
    否则，就会导致难以理解的错误信息，或者是产生了有效的代码却导致了意料之外的行为。
    
    基于这一原因，C++语言的设计者们一直在致力于实现一种能够为模板参数显式地提供（或者是检查）接口的能力。
      在 C++中这一接口被称为 concept。它代表了为了能够成功的实例化模板，模板参数必须要满足的一组约束条件。
      
    尽管已经在这一领域耕耘了很多年，concept 却依然没有被纳入 C++17。
        不过目前已有一些编译器对这一特性做了实验性的支持，它也很可能会被纳入到紧随 C++17 之后的标准中。
        
    Concept 可以被理解成静态多态的一类“接口”。
    在我们的例子中，可能会像下面这样：
        #include "coord.hpp"
        template<typename T>
        concept GeoObj = requires(T x) {
            { x.draw() } -> void;
            { x.center_of_gravity() } -> Coord;
        };
        
    在这里我们使用关键字 concept 定义了一个 GeoObj concept，
      它要求一个类型要有可被调用的成员函数 draw()和 center_of_gravity()，同时也对它们的返回类型做了限制。
      
    现在我们可以重写样例模板中的一部分代码，
    以在其中使用 requires 子句要求模板参数满足 GeoObj concept：
    
        #include "conceptsreq.hpp"
        #include <vector>
        // draw any GeoObj
        template<typename T>
        requires GeoObj<T>
        void myDraw (T const& obj)
        {
            obj.draw();                                             // call draw() according to type of object
        }
        
        // compute distance of center of gravity between two GeoObjs
        template<typename T1, typename T2>
        requires GeoObj<T1> && GeoObj<T2>
        Coord distance (T1 const& x1, T2 const& x2)
        {
            Coord c = x1.center_of_gravity() - x2.center_of_gravity();
            return c.abs();                                         // return coordinates as absolute values
        }
        
        // draw homogeneous collection of GeoObjs
        template<typename T>
        requires GeoObj<T>
        void drawElems (std::vector<T> const& elems)
        {
            for (std::size_type i=0; i<elems.size(); ++i) {
                elems[i].draw();                                    // call draw() according to type of element
            }
        }
        
    对于那些可以参与到静态多态行为中的类型，该方法依然是非侵入的：
        // concrete geometric object class Circle
        // - not derived from any class or implementing any interface
        class Circle {
        public:
            void draw() const;
            Coord center_of_gravity() const;
        };
        
    也就是说，这一类类型的定义中依然不包含特定的基类，或者 require 子句，
       而且它们也依然可以是基础数据类型或者来自独立框架的类型。
       
    附录 E 中包含了对于 C++中的 concept 更为详细的讨论，因为它们被期望能够出现在下一个C++标准中。
 */

namespace ch18_4 {

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
main_Using_Concepts_18_4()
{

    return 0;
}