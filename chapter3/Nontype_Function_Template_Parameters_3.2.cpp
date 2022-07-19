//
// Created by yangfeng on 2022/7/18.
//
#include "../headers.h"

using namespace std;

/*
 * 同样也可以给函数模板定义非类型模板参数。
 * 当该类函数或操作是被用作其它函数的参数时，可能会很有用。
 * 比如当使用 C++标准库给一个集合中的所有元素增加某个值的时候，
 *   可以将这个函数模板的一个实例化版本用作第 4个参数：
 *   std::transform (source.begin(), source.end(),   //start and end of source
 *      dest.begin(),    //start of destination
 *      addValue<5,int>   // operation
 *      );
 */

namespace ch3_2 {
    
    template <int Val, typename T>
    T addValue(T x) {
        return x + Val;
    }
    
    /*
     * 同样也可以基于前面的模板参数推断出当前模板参数的类型。
     *   比如可以通过传入的非类型模板参数推断出返回类型。
     */
    template <auto Val, typename T = decltype(Val)>
    T foo() {
        cout << __func__ << endl;
    }
    
    /*
     * 或者可以通过如下方式确保传入的非类型模板参数的类型和类型参数的类型一致。
     */
    template <typename T, T Val = T{}>
    T bar() {
        cout << __func__ << "\t\t" << Val << endl;
        return Val;
    }

    class Tmp {
    public:
    
    };

}

int
//main()
main_nontype_function_template_parameters()
{
    ch3_2::bar<int, 3>();

    return 0;
}
