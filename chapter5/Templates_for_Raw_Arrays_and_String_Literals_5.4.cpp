//
// Created by yangfeng on 2022/7/20.
//
#include "../headers.h"

using namespace std;

/*
当向模板传递裸数组或者字符串常量时，需要格外注意以下内容：
    第一，如果参数是按引用传递的，那么参数类型不会退化（decay）。
        也就是说当传递”hello”作为参数时，模板类型会被推断为 char const[6]。
    这样当向模板传递长度不同的裸数组或者字符串常量时就可能遇到问题，因为它们对应的模板类型不一样。
    只有当按值传递参数时，模板类型才会退化（decay），这样字符串常量会被推断为 char const *。
*/

namespace ch5_4 {

    class Tmp {
    public:
    
    };

}

int
//main()
main_templates_for_raw_arrays_and_string_literals()
{

    return 0;
}
