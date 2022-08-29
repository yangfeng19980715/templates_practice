//
// Created by yangfeng on 2022/7/12.
//
#include "../headers.h"

using namespace std;

namespace ch1_4 {
  class Test {
  public:
    template<typename T1, typename T2, typename RT =
      std::decay_t<decltype(true ? T1() : T2())>
    > // decay_t确保返回的类型不是引用类型
    RT max(T1 a, T2 b) {
      return b < a ? a : b;
    }
    
    template<typename T1, typename T2, typename RT =
      std::common_type_t<T1(), T2()>
      > // std::common_type_t确保返回的类型不是引用类型,它会退化
      RT max_2(T1 a, T2 b) {
        return b < a ? a : b;
      }
  };
}

int
main_default_args_template_1_4()
//main()
{
  cout << "hello, world" << endl;
  
  return 0;
}