//
// Created by yangfeng on 2022/7/29.
//

#include "../headers.h"

/*
破译大篇幅的错误信息
    常规函数的编译错误信息通常非常简单且直中要点。比如当编译器报错说”class X has nomember ‘fun’”时，
     找到代码中相应的错误并不会很难。但是模板并不是这样。看下面这些例子。
     
    简单的类型不匹配情况
      考虑下面这个使用了 C++标准库的简单例子：
      
        #include <string>
        #include <map>
        #include <algorithm>
        int main()
        {
            std::map<std::string,double> coll;
            …
            // find the first nonempty string in coll:
            auto pos = std::find_if (coll.begin(), coll.end(), [] (std::string
                                                    const& s){return s != ""; });
        }
        
    其中有一个相当小的错误：一个 lambda 函数被用来找到第一个匹配的字符串，
    它依次将map 中的元素和一个字符串比较。
    但是，由于 map 中的元素是 key/value 对，因此传入 lambda的元素也将是一个 std::pair<std::string const, double>，
      而它是不能直接和字符串进行比较的。
 */

using namespace std;

namespace ch9_4 {

    class Tmp {
    public:
    
    };

}

int
//main()
main_decoding_the_error_novel()
{

    return 0;
}