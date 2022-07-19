//
// Created by yangfeng on 2022/7/16.
//
#include "../headers.h"

using namespace std;

namespace ch2_3 {
    
    template <typename T>
    class Test;
    
    template< typename T>
    std::ostream& operator<<(std::ostream&, Test<T> const & );
    
    template<typename T>
    class Test {
    public:
        int m_num;
    public:
        Test(int _num = 0) : m_num(_num) {  }
        static_assert(std::is_default_constructible<T>::value,
                "class Test requires default-constructible elements");
        // 友元
        // 重载<< 运算符，输出
        
        //方法一：先声明，再定义， 或者直接定义。这种情况会很复杂
        /*
        friend ostream& operator<<(ostream & strm, Test<T> const& t) {
            strm << t.m_num;
            return strm;
        }
        */
        
        // 方法二： 隐式地声明一个新的模板函数，但必须使用一个不同于类模板的模板参数
        /*
        template <typename U>
        friend ostream& operator<<(ostream& strm, Test<U> const & t) {
            strm << t.m_num;
            return strm;
        }
        */
        
        // 方法三：先将Test<T>的operator << 声明为一个模板，这要求先对Test<T>声明
        //friend std::ostream& operator<< <T> (ostream&, Test<T> const & );
        
        
    
    };
    
    class NoDefaultContructorClass {
    public:
        NoDefaultContructorClass() = delete;
    };
    
}

int
//main()
main_partially_use_class_template()
{
    ch2_3::Test<int> a;
    //ch2_3::Test<ch2_3::NoDefaultContructorClass> b; // failure
    
    return 0;
}
