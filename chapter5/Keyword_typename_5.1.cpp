//
// Created by yangfeng on 2022/7/20.
//
#include "../headers.h"

using namespace std;

/*
 *   关键字 typename 在 C++标准化过程中被引入进来，
 *       用来澄清模板内部的一个标识符代表的是某种类型，而不是数据成员。
 *
 *   通常而言，当一个依赖于模板参数的名称代表的是某种类型的时候，就必须使用 typename。
 */

namespace ch5_1 {
    
    class MyIteratorClass {
    public:
        template <typename T>
        void printColl(T const & coll) {
            // const_iterator 是只读迭代器，不能往里面写入
            typename T::const_iterator pos;
            typename T::const_iterator end(coll.end());
            
            for (pos = coll.begin(); pos != end; ++pos)
                std::cout << *pos << '\t';
            
            std::cout << endl;
            
        }
    };

    class Tmp {
    public:
    
    };

}

int
//main()
main_keyword_typename()
{
    ch5_1::MyIteratorClass mic;
    vector<int> v{1, 2, 3, 4, 5};
    mic.printColl(v);

    return 0;
}
