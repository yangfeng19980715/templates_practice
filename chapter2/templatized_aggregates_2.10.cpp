//
// Created by yangfeng on 2022/7/18.
//
#include "../headers.h"

using namespace std;

/*
 * 聚合类（这样一类class或者struct：没有用户定义的显示的，或者继承而来的构造函数，没有private
 * 或者protected的非静态成员，没有虚函数，没有virtual，private或者protected的基类）也可以是模板。
 * 比如 class ValueWithComment
 */

namespace ch2_10 {

    /*
     * 定义了一个成员value类型被参数化了的聚合类，可以像定义其他类模板的对象一样定义一个聚合类的对象。
     * ValueWithComment<int> vc;
     * vc.value = 42;
     * vc.comment = "initial comment";
     *
     * 从c++17开始，对于聚合类的类模板甚至可以使用推断指引。
     * ValueWithComment(char const *, char const *) -> ValueWithComment<std::string>;
     * ValueWithComment vc2 = {"hello", "world"};
     *
     * 聚合类可以用纯右值实质初始化聚合类成员，一旦用了构造函数就不一样了。
     *
     */
    template <typename T>
    struct ValueWithComment {
        T value;
        std::string comment;
    
    };
    
    ValueWithComment(char const *, char const *)
            -> ValueWithComment<std::string>;
    
    void test_value_with_coomment() {
        ValueWithComment<int> vc;
        vc.value = 33;
        vc.comment = "initial comment";
        
        ValueWithComment vc2 = {"hello", "world"};
        cout << vc2.value << ", " << vc2.comment << endl;
    }

}

int
//main()
main_templatized_aggregates()
{
    ch2_10::test_value_with_coomment();
    //std::array<int, 4> arr {1, 2, 3, 4};
    std::array<int, 4> arr;
    std::iota(arr.begin(), arr.end(), 5);
    //std::for_each(begin(arr), end(arr), [](int i) { cout << i << '\t'; });
    

    return 0;
}