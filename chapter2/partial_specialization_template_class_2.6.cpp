//
// Created by yangfeng on 2022/7/16.
//
#include "../headers.h"

using namespace std;

/**
 * 类模板可以只被部分特例化，这样就可以为某些特殊情况提供特殊的实现，
 * 不过使用者还是要定义一部分模板参数。
 * 比如，可以特殊化一个stack<>来专门处理指针
 */

namespace ch2_6 {
    template <typename T>
    class stack {
    private:
        std::vector<T> elems;
    public:
        void push(T const & t) {
        
        }
    
    };
    

    template<typename T>
    class stack<T*> {
    private:
        std::vector<T*> elems;
    public:
        void push(T const * t);
    
    };
    
    template<typename T>
    void stack<T*>::push(const T *t) {
    
    }
    
    /**
     * 类模板也可以特例化多个模板参数之间的关系
     */
     template<typename T1, typename T2>
     class MyClass {
     
     };
    
    // 一
    template<typename T>
    class MyClass<T, int> {
    
    };
    
     // 二
    template<typename T1, typename T2>
    class MyClass<T1*, T2*> {
    
    };
    
    
}

int
//main()
main_partial_specialized_template_class()
{

    return 0;
}
