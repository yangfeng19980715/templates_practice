//
// Created by yangfeng on 2022/8/1.
//

#include "../headers.h"

/*
静态多态
    模板也可以被用来实现多态。
    不同的是，它们不依赖于对基类中公共行为的分解。
    取而代之的是，这一“共性（commonality）”隐式地要求不同的“形状（shapes）”必须支持使用了相同语法的操作（比如，相关函数的名字必须相同）。
    在定义上，具体的 class 之间彼此相互独立（参见 18.2）。
     在用这些具体的 class 去实例化模板的时候，这一多态能力得以实现。
 */

using namespace std;

/*
比如，上一节中的 myDraw():
    void myDraw (GeoObj const& obj)             // GeoObj is abstract base class
    {
        obj.draw();
    }
也可以被实现成下面这样：
    template<typename GeoObj>
    void myDraw (GeoObj const& obj)             // GeoObj is template parameter
    {
        obj.draw();
    }
    
比较 myDraw()的两种实现，可以发现其主要的区别是将 GeoObj 用作模板参数而不是公共基类。
 但是，在表象之下还有很多区别。
 比如，使用动态多态的话，在运行期间只有一个 myDraw() 函数，但是在使用模板的情况下，
   却会有多种不同的函数，例如 myDraw<Line>()和 myDraw<Circle>()。
 */

namespace ch18_2 {

    namespace case1 {
    
        class Circle {
        public:
            void draw() const { }
            int center_of_gravity() const { return 0; }
        };
        
        class Line {
        public:
            void draw() const { }
    
            int center_of_gravity() const { return 0; }
        };
        
        class Coord {
        public:
            Coord(int) {  }
            Coord abs() { return Coord{ 3 }; }
        };
    
        template<typename GeoObj>
        void myDraw (GeoObj const& obj) { obj.draw(); }
        
        template<typename GeoObj1, typename GeoObj2>
        Coord distance (GeoObj1 const& x1, GeoObj2 const& x2)
        {
            Coord c = x1.center_of_gravity() - x2.center_of_gravity();
            return c.abs();
        }
        
        template<typename GeoObj>
        void drawElems (std::vector<GeoObj> const& elems)
        {
            for (unsigned i=0; i<elems.size(); ++i) {
                elems[i].draw(); // call draw() according to type of element
            }
        }
        
        void test_Static_Polymorphism() {
            Line l;
            Circle c, c1, c2;
            myDraw(l); // myDraw<Line>(GeoObj&) => Line::draw()
            myDraw(c); // myDraw<Circle>(GeoObj&) => Circle::draw()
            distance(c1,c2); //distance<Circle,Circle> (GeoObj1&,GeoObj2&)
            distance(l,c); // distance<Line,Circle> (GeoObj1&,GeoObj2&) collection possible
            std::vector<Line> coll; // OK: homogeneous collection possible
            coll.push_back(l); // insert line
            drawElems(coll); // draw all lines
        }
        
        /*
         和 myDraw()类似，我们不能够再将 GeoObj 作为具体的参数类型用于 distance()。
         我们引入了两个模板参数，GeoObj1 和 GeoObj2，来支持不同类型的集合对象之间的距离计算：
                distance(l,c); // distance<Line,Circle>(GeoObj1&,GeoObj2&)
                
        但是使用这种方式，我们将不再能够透明地处理异质容器。
         这也正是 static 多态中的 static部分带来的限制：
            所有的类型必须在编译期可知。
         不过，我们可以很容易的为不同的集合对象类型引入不同的集合。
         这样就不再要求集合的元素必须是指针类型，这对程序性能和类型安全都会有帮助。
         */
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
main_Static_Polymorphism_18_2()
{

    return 0;
}