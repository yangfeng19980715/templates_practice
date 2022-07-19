//
// Created by yangfeng on 2022/7/12.
//
#include "../headers.h"

using namespace std;

namespace ch1_5 {
    int max(int a, int b) {
        return a > b ? a : b;
    }
    
    template <typename T>
    T max(T a, T b) {
        return a > b ? a : b;
    }
    
    template<typename T1, typename T2>
    auto max(T1 a, T2 b) {
        return a > b ? a : b;
    }
    
    template<typename RT, typename T1, typename T2>
    RT max(T1 a, T2 b) {
        return a > b ? a : b;
    }
    
    // 可以显示指定一个空的模板列表，这表明它会被解析成一个模板调用，其所有的模板参数会被通过
    // 调用参数推断出来
    void test_no_arg() {
        cout << ch1_5::max<>(3, 4) << endl;
    }
    
}

int
//main()
main_overload_function_template_1_5()
{
    ch1_5::test_no_arg();
    
    return 0;
}
