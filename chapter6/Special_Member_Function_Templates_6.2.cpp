//
// Created by yangfeng on 2022/7/21.
//
#include "../headers.h"

/* 特殊成员函数也可以是模板，比如构造函数，但是有时候这可能会带来令人意外的结果。 */

namespace ch6_2 {
    
    namespace case1 {
        class Person
        {
        public:
            std::string name;
        public:
            // constructor for passed initial name:
            explicit Person(std::string const& n) : name(n) {
                std::cout << "copying string-CONSTR for " << name << std::endl;
            }
    
             explicit Person(std::string&& n) : name(std::move(n)) {
                std::cout << "moving string-CONSTR for ’" << name << "’\n";
            }
            // copy and move constructor:
            Person (Person const& p) : name(p.name) {
                std::cout << "COPY-CONSTR Person ’" << name << "’\n";
            }
            Person (Person&& p) : name(std::move(p.name)) {
                std::cout << "MOVE-CONSTR Person ’" << name << "’\n";
            }
        };
    
         void Person_main() {
            std::string s = "str1";
            Person p1(s);                   // init with string object => calls copying string-CONSTR
            //Person p2("tmp");             // init with string literal => calls moving string-CONSTR
            //Person p3(p1);                // copy Person => calls COPY-CONSTR
            //Person p4(std::move(p1));     // move Person => calls MOVE-CONST
            //std::string mys = "my string";
            //Person p5{ std::move(mys) };
        }
    }
    
    namespace case2 {
        class Person
        {
        private:
            std::string name;
        public:
            
            // generic constructor for passed initial name:
            template<typename STR>
            explicit Person(STR&& n) : name(std::forward<STR>(n)) { std::cout << "TMPL-CONSTR for " << name << "\n"; }
            
            // copy and move constructor:
            Person (Person const& p) : name(p.name) { std::cout << "COPY-CONSTR Person ’" << name << "’\n"; }
            
            Person (Person&& p) : name(std::move(p.name))  { std::cout << "MOVE-CONSTR Person ’" << name << "’\n"; }
        };
        
        void Person_main() {
            std::string s = "sname";
            Person p1(s);
            Person p2("tmp");
            //Person p3 { p1 };
        }
        /*
        注意这里在构建 p2 的时候并不会创建一个临时的 std::string 对象：
             STR 的类型被推断为 char const[4](万能引用也是引用，参数类型不会退化)。
             
         但是将 std::forward<STR>用于指针参数没有太大意义。
         成员 name 将会被一个以null 结尾的字符串构造。
         
        但是，当试图调用拷贝构造函数的时候，会遇到错误：
            Person p3(p1); // ERROR
            
        而用一个可移动对象初始化 Person 的话却可以正常工作：
            Person p4(std::move(p1)); // OK: move Person => calls MOVECONST
            
        如果试图拷贝一个 Person 的 const 对象的话，也没有问题：
            Person const p2c("ctmp"); //init constant object with string literal
            Person p3c(p2c); // OK: copy constant Person => calls COPY-CONSTR
            
        问题出在这里：
         根据 C++重载解析规则（参见 16.2.5 节），对于一个非 const 左值的 Person p，成员模板
            template<typename STR>
            Person(STR&& n)
        通常比预定义的拷贝构造函数更匹配：
            Person (Person const& p)
            
        这里 STR 可以直接被替换成 Person&，但是对拷贝构造函数还要做一步 const 转换。
        额外提供一个非 const 的拷贝构造函数看上去是个不错的方法：
            Person (Person& p)
        不过这只是一个部分解决问题的方法，更好的办法依然是使用模板。
        我们真正想做的是当参数是一个 Person 对象或者一个可以转换成 Person 对象的表达式时，不要启用模板。
        这可以通过 std::enable_if<>实现，它也正是下一节要讲的内容。
        */
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_special_member_function_templates()
{
    ch6_2::case2::Person_main();

    return 0;
}
