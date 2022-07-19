//
// Created by yangfeng on 2022/7/16.
//
#include "../headers.h"

using namespace std;

/**
 * 和函数模板一样，也可以给类模板的模板参数指定默认值，比如对stack<>，
 * 可以将其用来容纳元素的容器声明为第二个模板参数，并指定其默认值是std::vector<>
 */

namespace nmsp {

    template <typename T, typename Cont = std::vector<T>>
    class stack {
    private:
        Cont elems;
    public:
        void push(T const & elem);
    
    };
    
    template <typename T, typename Cont>
    void stack<T, Cont>::push(const T &elem) {
        elems.push_back(elem);
    }
    
    

}

int
//main()
main_default_template_args()
{

    return 0;
}
