//
// Created by yangfeng on 2022/7/20.
//
#include "../headers.h"

using namespace std;

/*
 * 如果允许模板参数也是一个类模板的话，会有不少好处。
 *
 使用模板参数模板，在声明 Stack 类模板的时候就可以只指定容器的类型而不去指定容器中元素的类型：
    Stack<int, std::vector> vStack; // integer stack that uses a vector
    
    为此就需要在 Stack 的定义中将第二个模板参数声明为模板参数模板。
    可能像下面这样：
        template<typename T,
        template<typename Elem> class Cont = std::deque>
        class Stack {
        private:
            Cont<T> elems;              // elements
        public:
            void push(T const&);        // push element
            void pop();                 // pop element
            T const& top() const;       // return top element
            bool empty() const {        // return whether the stack is empty
                return elems.empty();
            }
            …
        };
        
        区别在于第二个模板参数被定义为一个类模板：
            template<typename Elem> class Cont
            
        默认类型也从 std::deque<T>变成 std::deque。
        这个参数必须是一个类模板，它将被第一个模板参数实例化：
            Cont<T> elems;
        用第一个模板参数实例化第二个模板参数的情况是由 Stack 自身的情况决定的。
        实际上，可以在类模板内部用任意类型实例化一个模板参数模板。
        
        由于模板参数模板中的模板参数没有被用到，作为惯例可以省略它（除非它对文档编写有帮助）：
            template<typename T, template<typename> class Cont = std::deque>
            class Stack {
                …
            };
        
        成员函数也要做相应的更改。必须将第二个模板参数指定为模板参数模板。
        比如对于 push()成员，其实现如下：
            template<typename T, template<typename> class Cont>
            void Stack<T,Cont>::push (T const& elem)
            {
                elems.push_back(elem); // append copy of passed elem
            }
        注意，虽然模板参数模板是类或者别名类（alias templates）的占位符，但是并没有与其对
        应的函数模板或者变量模板的占位符。
 *
 */

/*
 模板参数模板的参数匹配
    如果你尝试使用新版本的 Stack，可能会遇到错误说默认的 std::deque 和模板参数模板 Cont不匹配。
    这是因为在 C++17 之前，
    template<typename Elem> typename Cont = std::deque 中的模板参数必须
        和实际参数（std::deque）的模板参数匹配（对变参模板有些例外）。
    而且实际参数（std::deque 有两个参数，第二个是默认参数 allocator）的默认参数也要被匹配，
    这样 template<typename Elem> typename Cont = std::dequ 就不满足以上要求（不过对 C++17 可以）。
    
    作为变通，可以将类模板定义成下面这样：
    
        template<typename T, template<typename Elem,
        typename Alloc = std::allocator<Elem>> class Cont = std::deque>
        class Stack {
        private:
            Cont<T> elems; // elements
            …
        };
    其中的 Alloc 同样可以被省略掉。
  
  
 */


namespace ch5_7 {

    class Tmp {
    public:
    
    };

}

int
//main()
main_template_template_parameters()
{

    return 0;
}
