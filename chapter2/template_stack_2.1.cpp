//
// Created by yangfeng on 2022/7/13.
//
#include "../headers.h"

using namespace std;

class Test {
public:
  int m_num;
  Test() { cout << "Test()" << endl; }
  Test(int _num) : m_num(_num) { cout << "Test(int)" << endl; }
};


void
func_template_stack_2_1()
{
  shared_ptr<Test> A{new Test{ 1 }};
  map<int, shared_ptr<Test>> mp;
  
  // 这里有一个之前已经存在的数据 A , 类型是 shared_ptr<Test>
  mp[1] = make_shared<Test>();
  mp[1] = A;
  
  
  cout << "end" << endl;
}

int
//main()
main_template_stack()
{
  cout << "hello, world" << endl;
  
  return 0;
}
