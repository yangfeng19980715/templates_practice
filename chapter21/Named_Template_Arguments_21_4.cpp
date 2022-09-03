//
// Created by yangfeng on 8/31/2022.
//
#include "../headers.h"

using namespace std;

/*
21.4 Named Template Arguments（命名的模板参数）
不少模板技术有时会导致类模板包含很多不同的模板类型参数。但是，其中一些模板参数通
常都会有合理的默认值。其中一种这一类模板的定义方式可能会向下面这样：
template<typename Policy1 = DefaultPolicy1,
typename Policy2 = DefaultPolicy2,
typename Policy3 = DefaultPolicy3,
typename Policy4 = DefaultPolicy4>
class BreadSlicer {
…
};
可以想象，在使用这样一个模板时通常都可以使用模板参数的默认值。但是，如果需要指定
某一个非默认参数的值的话，那么也需要指定该参数前面的所有参数的值（虽然使用的可能
五车书馆
289
是它们的默认值）。
很 显 然 ， 我 们 更 倾 向 于 使 用 BreadSlicer<Policy3 = Custom> 的 形 式 ， 而 不 是
BreadSlicer<DefaultPolicy1, DefaultPolicy2, Custom>。在下面的内容在，我们开发了一种几乎
可以完全实现以上功能的技术。
我们的技术方案是将默认类型放在一个基类中，然后通过派生将其重载。相比与直接指定类
型参数，我们会通过辅助类（helper classes）来提供相关信息。比如我们可以将其写成这样
BreadSlicer<Policy3_is<Custom>>。由于每一个模板参数都可以表述任一条款，默认值就不能
不同。或者说，在更高的层面上，每一个模板参数都是等效的：
template<typename PolicySetter1 = DefaultPolicyArgs,
typename PolicySetter2 = DefaultPolicyArgs,
typename PolicySetter3 = DefaultPolicyArgs,
typename PolicySetter4 = DefaultPolicyArgs>
class BreadSlicer {
using Policies = PolicySelector<PolicySetter1,
PolicySetter2,
PolicySetter3,
PolicySetter4>;
// use Policies::P1, Policies::P2, … to refer to the various policies
…
};
剩余的挑战就是该如何设计 PolicySelector 模板了。必须将不同的模板参数融合进一个单独
的类型，而且这个类型需要用那个没有指定默认值的类型去重载默认的类型别名成员。可以
通过继承实现这一融合：
// PolicySelector<A,B,C,D> creates A,B,C,D as base classes
// Discriminator<> allows having even the same base class more than once
template<typename Base, int D>
class Discriminator : public Base {
};
template<typename Setter1, typename Setter2,
typename Setter3, typename Setter4>
class PolicySelector : public Discriminator<Setter1,1>,
public Discriminator<Setter2,2>,
public Discriminator<Setter3,3>,
public Discriminator<Setter4,4>
{
};
注意此处对中间的 Discriminator 模板的使用。其要求不同的 Setter 类型是类似的（不能使用
多个类型相同的直接基类。而非直接基类，则可以使用和其它基类类似的类型）。
五车书馆
290
正如之前提到的，我们将全部的默认值收集到基类中：
// name default policies as P1, P2, P3, P4
class DefaultPolicies {
public:
using P1 = DefaultPolicy1;
using P2 = DefaultPolicy2;
using P3 = DefaultPolicy3;
using P4 = DefaultPolicy4;
};
但是，如果我们最终会从该基类继承很多次的话，需要额外小心的避免歧义。因此，此处需
要确保对基类使用虚继承：
// class to define a use of the default policy values
// avoids ambiguities if we derive from DefaultPolicies more than once
class DefaultPolicyArgs : virtual public DefaultPolicies {
};
最后，我们也需要一些模板来重载掉那些默认的策略值：
template<typename Policy>
class Policy1_is : virtual public DefaultPolicies {
public:
using P1 = Policy; // overriding type alias
};
template<typename Policy>
class Policy2_is : virtual public DefaultPolicies {
public:
using P2 = Policy; // overriding type alias
};
template<typename Policy>
class Policy3_is : virtual public DefaultPolicies {
public:
using P3 = Policy; // overriding type alias
};
template<typename Policy>
class Policy4_is : virtual public DefaultPolicies {
public:
using P4 = Policy; // overriding type alias};
}
有了 Discriminator<>类模板的帮助，这就会产生一种层级关系，在其中所有的模板参数都是
基类（参见图 21.4）。重要的一点是，所有的这些基类都有一个共同的虚基类 DefaultPolicies，
五车书馆
291
也正是它定义了 P1，P2，P3 和 P4 的默认值。但是 P3 在某一个派生类中被重新定义了（比
如在 Policy3_is<>中）。根据作用域规则，该定义会隐藏掉在基类中定义的相应定义。这样，
就不会有歧义了。
Figure 21.4. Resulting type hierarchy of BreadSlicer<>::Policies
在模板 BreadSlicer 中，可以使用 Policies::P3 的形式引用以上 4 中策略。比如:
template<…>
class BreadSlicer {
…
public:
void print () {
Policies::P3::doPrint();
}
…
};
在 inherit/namedtmpl.cpp 中，可以找到完整的例子。
虽然在上面开发的例子中只用到了四个模板类型参数，但是很显然该技术适用于任意数量的
模板参数。注意，我们实际上永远不会对包含虚基类的辅助类进行实例化。因此，虽然它们
是虚基类，但是并不会导致性能或者内存消耗的问题。
 */

namespace ch21_4 {

  namespace case1 {
  
    /*
  
    // PolicySelector<A,B,C,D> creates A,B,C,D as base classes
    // Discriminator<> allows having even the same base class more than once
    template<typename Base, int D>
    class Discriminator : public Base { };
  
    template<typename Setter1, typename Setter2, typename Setter3, typename Setter4>
    class PolicySelector : public Discriminator<Setter1,1>,
                           public Discriminator<Setter2,2>,
                           public Discriminator<Setter3,3>,
                           public Discriminator<Setter4,4>
    { };
  
    // 正如之前提到的，我们将全部的默认值收集到基类中：
    // name default policies as P1, P2, P3, P4
    // template <typename DefaultPolicy1, typename DefaultPolicy2, typename DefaultPolicy3, typename DefaultPolicy4>
    class DefaultPolicies {
    public:
      using P1 = DefaultPolicy1;
      using P2 = DefaultPolicy2;
      using P3 = DefaultPolicy3;
      using P4 = DefaultPolicy4;
    };
  
    // 但是，如果我们最终会从该基类继承很多次的话，需要额外小心的避免歧义。因此，此处需要确保对基类使用虚继承：
    // class to define a use of the default policy values
    // avoids ambiguities if we derive from DefaultPolicies more than once
    class DefaultPolicyArgs : virtual public DefaultPolicies {
    };
    
    template<typename PolicySetter1 = DefaultPolicyArgs,
      typename PolicySetter2 = DefaultPolicyArgs,
      typename PolicySetter3 = DefaultPolicyArgs,
      typename PolicySetter4 = DefaultPolicyArgs>
    class BreadSlicer {
      using Policies = PolicySelector<PolicySetter1,
        PolicySetter2,
        PolicySetter3,
        PolicySetter4>;
        // use Policies::P1, Policies::P2, … to refer to the various policies
    };
  
    // 最后，我们也需要一些模板来重载掉那些默认的策略值：
    template<typename Policy>
    class Policy1_is : virtual public DefaultPolicies {
    public:
      using P1 = Policy; // overriding type alias
    };
    
    template<typename Policy>
    class Policy2_is : virtual public DefaultPolicies {
    public:
      using P2 = Policy; // overriding type alias
    };
    
    template<typename Policy>
    class Policy3_is : virtual public DefaultPolicies {
    public:
      using P3 = Policy; // overriding type alias
    };
    
    template<typename Policy>
    class Policy4_is : virtual public DefaultPolicies {
    public:
      using P4 = Policy; // overriding type alias};
    };
     */
    
    
    /*
     剩余的挑战就是该如何设计 PolicySelector 模板了。必须将不同的模板参数融合进一个单独
的类型，而且这个类型需要用那个没有指定默认值的类型去重载默认的类型别名成员。可以
通过继承实现这一融合：
     */
  
    
    void test() {
      cout << "hello, world" << endl;
    }
    
  }
  
  namespace case2 {
  
    void test() {
    
    }
    
  }
  
  namespace case3 {
  
    void test() {
    
    }
    
  }
  
  namespace case4 {
  
    void test() {
    
    }
    
  }
  
  namespace case5 {
  
    void test() {
    
    }
  
  }

  class Tmp {
  public:
  
  };

}

int
//main()
main_ch21_4()
{
  cout << "----------------case1::test()  start---------------------" << endl;
  ch21_4::case1::test();
  cout << "----------------case1::test()  end---------------------" << endl << endl;
  cout << "----------------case2::test()  start---------------------" << endl;
  ch21_4::case2::test();
  cout << "----------------case2::test()  end---------------------" << endl << endl;
  cout << "----------------case3::test()  start---------------------" << endl;
  ch21_4::case3::test();
  cout << "----------------case3::test()  end---------------------" << endl;
  cout << "----------------case4::test()  start---------------------" << endl << endl;
  ch21_4::case4::test();
  cout << "----------------case4::test()  end---------------------" << endl;
  cout << "----------------case5::test()  start---------------------" << endl << endl;
  ch21_4::case5::test();
  cout << "----------------case5::test()  end---------------------" << endl << endl;

  return 0;
}