//
// Created by yangfeng on 2022/7/19.
//
#include "../headers.h"

using namespace std;

/*  变参模板在泛型库的开发中有重要的作用，比如 C++标准库。
 *     一个重要的作用是转发任意类型和数量的参数。
 *     通常是使用移动语义对参数进行完美转发（perfectly forwarded）。
 */

/*
 比如在如下情况下会使用这一特性：
 
     向一个由智能指针管理的，在堆中创建的对象的构造函数传递参数：
    // create shared pointer to complex<float> initialized by 4.2 and 7.7:
    auto sp = std::make_shared<std::complex<float>>(4.2, 7.7);
    
     向一个由库启动的 thread 传递参数：
    std::thread t (foo, 42, "hello"); //call foo(42,"hello") in a separate thread
    
     向一个被 push 进 vector 中的对象的构造函数传递参数：
    std::vector<Customer> v;
    v.emplace("Tim", "Jovi", 1962); //insert a Customer initialized by three arguments
 */

namespace ch4_3 {

    class Tmp {
    public:
    
    
    };

}

int
//main()
main_application_of_variadic_templates()
{

    return 0;
}
