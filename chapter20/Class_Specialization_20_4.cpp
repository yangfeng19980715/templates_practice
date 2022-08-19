//
// Created by yangfeng on 2022/8/13.
//

#include "../headers.h"

/*
20.4 类的特化（Class Specialization）
    类模板的偏特化可以被用来提供一个可选的、为特定模板参数进行了特化的实现，这一点和
    函数模板的重载很相像。而且，和函数模板的重载类似，如果能够基于模板参数的属性对各
    种偏特化版本进行区分，也会很有意义。考虑一个以 key 和 value 的类型为模板参数的泛型
    Dictionary 类模板。只要 key 的类型提供了 operator==()运算符，就可以实现一个简单（但是
    低效）的 Dictionary：
    template<typename Key, typename Value>
    class Dictionary
    {
    private:
    vector<pair<Key const, Value>> data;
    public:
    //subscripted access to the data:
    value& operator[](Key const& key)
    {
    // search for the element with this key:
    for (auto& element : data) {
    if (element.first == key){
    return element.second;
    }
    }
    // there is no element with this key; add one
    data.push_back(pair<Key const, Value>(key, Value()));
    五车书馆
    256
    return data.back().second;
    }
    ...
    };
    如果 key 的类型提供了 operator <()运算符的话，则可以基于标准库的 map 容器提供一种相
    对高效的实现方式。类似的，如果 key 的类型提供了哈希操作的话，则可以基于标准库的
    unordered_map 提供一种更为高效的实现方式。
    20.4.1 启用/禁用类模板
    启用/禁用类模板的不同实现方式的方法是使用类模板的偏特化。为了将 EnableIf 用于类模
    板的偏特化，需要先为 Dictionary 引入一个未命名的、默认的模板参数：
    template<typename Key, typename Value, typename = void>
    class Dictionary
    {
    ... //vector implementation as above
    };
    这个新的模板参数将是我们使用 EnableIf 的入口，现在它可以被嵌入到基于 map 的偏特化
    Dictionary 的模板参数例表中：
    template<typename Key, typename Value>
    class Dictionary<Key, Value, EnableIf<HasLess<Key>>>
    {
    private:
    map<Key, Value> data;
    public:value& operator[](Key const& key) {
    return data[key];
    }
    ...
    };
    和函数模板的重载不同，我们不需要对主模板的任意条件进行禁用，因为对于类模板，任意
    偏特化版本的优先级都比主模板高。但是，当我们针对支持哈希操作的另一组 keys 进行特
    化时，则需要保证不同偏特化版本间的条件是互斥的：
    template<typename Key, typename Value, typename = void>
    class Dictionary
    {
    ... // vector implementation as above
    };
    template<typename Key, typename Value>
    class Dictionary<Key, Value, EnableIf<HasLess<Key> && !HasHash<Key>>> {
    五车书馆
    257
    {
    ... // map implementation as above
    };
    template typename Key, typename Value>
    class Dictionary Key, Value, EnableIf HasHash Key>>>
    {
    private:
    unordered_map Key, Value> data;
    public:
    value& operator[](Key const& key) {
    return data[key];
    }
    ...
    };
    20.4.2 类模板的标记派发
    同样地，标记派发也可以被用于在不同的模板特化版本之间做选择。为了展示这一技术，我
    们定义一个类似于之前章节中介绍的 advanceIter()算法的函数对象类型 Advance<Iterator>，
    它同样会以一定的步数移动迭代器。会同时提供基本实现（用于 input iterators）和适用于双
    向迭代器和随机访问迭代器的特化版本，并基于辅助萃取 BestMatchInSet（下面会讲到）为
    相应的迭代器种类选择最合适的实现版本：
    // primary template (intentionally undefined):
    template<typename Iterator,
    typename Tag = BestMatchInSet< typename
    std::iterator_traits<Iterator> ::iterator_category,
    std::input_iterator_tag,
    std::bidirectional_iterator_tag,
    std::random_access_iterator_tag>>
    class Advance;
    // general, linear-time implementation for input iterators:
    template<typename Iterator>
    class Advance<Iterator, std::input_iterator_tag>
    {
    public:
    using DifferenceType = typename
    std::iterator_traits<Iterator>::difference_type;
    void operator() (Iterator& x, DifferenceType n) const
    {
    while (n > 0) {
    ++x;
    五车书馆
    258
    --n;
    }
    }
    };
    // bidirectional, linear-time algorithm for bidirectional iterators:
    template<typename Iterator>
    class Advance<Iterator, std::bidirectional_iterator_tag>
    {
    public:
    using DifferenceType =typename
    std::iterator_traits<Iterator>::difference_type;
    void operator() (Iterator& x, DifferenceType n) const
    {
    if (n > 0) {
    while (n > 0) {
    ++x;
    --n;
    }
    } else {
    while (n < 0) {
    --x;
    ++n;
    }
    }
    }
    };
    // bidirectional, constant-time algorithm for random access iterators:
    template<typename Iterator>
    class Advance<Iterator, std::random_access_iterator_tag>
    {
    public:
    using DifferenceType =
    typename std::iterator_traits<Iterator>::difference_type;
    void operator() (Iterator& x, DifferenceType n) const
    {
    x += n;
    }
    }
    这一实现形式和函数模板中的标记派发很相像。但是，比较困难的是 BestMatchInSet 的实现，
    它主要被用来为一个给定的迭代器选择选择最匹配 tag。本质上，这个类型萃取所做的是，
    当给定一个迭代器种类标记的值之后，要判断出该从以下重载函数中选择哪一个，并返回其
    五车书馆
    259
    参数类型：
    void f(std::input_iterator_tag);
    void f(std::bidirectional_iterator_tag);
    void f(std::random_access_iterator_tag);
    模拟重载解析最简单的方式就是使用重载解析，就像下面这样：
    // construct a set of match() overloads for the types in Types...:
    template<typename... Types>
    struct MatchOverloads;
    // basis case: nothing matched:
    template<>
    struct MatchOverloads<> {
    static void match(...);
    };
    // recursive case: introduce a new match() overload:
    template<typename T1, typename... Rest>
    struct MatchOverloads<T1, Rest...> : public MatchOverloads<Rest...>
    {
    static T1 match(T1); // introduce overload for T1
    using MatchOverloads<Rest...>::match;// collect overloads from bases
    };
    // find the best match for T in Types...
    template<typename T, typename... Types>
    struct BestMatchInSetT {
    using Type = decltype(MatchOverloads<Types...>::match(declval<T> ()));
    };
    template<typename T, typename... Types>
    using BestMatchInSet = typename BestMatchInSetT<T, Types...>::Type;
    MatchOverloads 模板通过递归继承为输入的一组 Types 中的每一个类型都声明了一个 match()
    函数。每一次递归模板 MatchOverloads 偏特化的实例化都为列表中的下一个类型引入了一
    个新的 match()函数。然后通过使用 using 声明将基类中的 match()函数引入当前作用域。当
    递归地使用该模板的时候，我们就有了一组和给定类型完全对应的 match()函数的重载，每
    一个重载函数返回的都是其参数的类型。然后 BestMatchInSetT 模板会将 T 类型的对象传递
    给一组 match()的重载函数，并返回最匹配的 match()函数的返回类型。如果没有任何一个
    match()函数被匹配上，那么返回基本情况对应的 void（使用省略号来捕获任意参数）将代
    表出现了匹配错误。总结来讲，BestMatchInSetT 将函数重载的结果转化成了类型萃取，这
    样可以让通过标记派发，在不同的模板偏特化之间做选择的情况变得相对容易一些。
 */

using namespace std;

namespace ch20_4 {
    
    namespace case1 {
        
        template<typename Key, typename Value>
        class Dictionary {
        private:
            vector<pair<Key const, Value>> data;
        
        public:
            //subscripted access to the data:
            Value &operator[](Key const &key) {
                // search for the element with this key:
                for (auto &element: data) {
                    if (element.first == key) {
                        return element.second;
                    }
                }
                
                // there is no element with this key; add one
                data.push_back(pair<Key const, Value>(key, Value()));
                return data.back().second;
            }
            
        };
    }
    
    /*
    namespace case2 {
    
        template<typename Key, typename Value, typename = void>
        class Dictionary
        {
        private:
            vector<pair<Key const, Value>> data;
    
        public:
            //subscripted access to the data:
            Value& operator[](Key const& key)
            {
                // search for the element with this key:
                for (auto& element : data) {
                    if (element.first == key) {
                        return element.second;
                    }
                }
            
                // there is no element with this key; add one
                data.push_back(pair<Key const, Value>(key, Value()));
                return data.back().second;
            }
        
        };
        
        template <typename T>
        struct HasLessT {
            static constexpr bool value = true;
        };
        
        template <bool cond>
        struct EnableIf { };
        
        template<typename T>
        using HasLess = HasLessT<T>::value;
        
        template<typename Key, typename Value>
        //class Dictionary<Key, Value, EnableIf<HasLess<Key>>>
        class Dictionary<Key, Value, std::enable_if<HasLess<Key>>>
        {
        
        private:
            map<Key, Value> data;
            
        public:
            
            Value& operator[](Key const& key) {
                return data[key];
            }
        
        };
    }
    
    namespace case3 {
        template<typename Key, typename Value, typename = void>
        class Dictionary
        {
            // vector implementation as above
        };
        
        template <typename T>
        struct HasHashT {
            static constexpr bool value = true;
        };
        
        template <typename T>
        using HasHash = typename HasHashT<T>::value;
        
    
        template<typename Key, typename Value>
        class Dictionary<Key, Value, case2::EnableIf<(case2::HasLess<Key> && !HasHash<Key>)>> {
        {
            // map implementation as above
        };
    
        template typename Key, typename Value>
        class Dictionary Key, Value, EnableIf HasHash Key>>>
        {
            private:
            unordered_map Key, Value> data;
            public:
            value& operator[](Key const& key) {
                return data[key];
            }
        };
    

    }
     */
    
    namespace case4 {
        
        
        /*
        这一实现形式和函数模板中的标记派发很相像。但是，比较困难的是 BestMatchInSet 的实现，
            它主要被用来为一个给定的迭代器选择选择最匹配 tag。
            
        本质上，这个类型萃取所做的是，当给定一个迭代器种类标记的值之后，要判断出该从以下重载函数中选择哪一个，并返回其参数类型：
            void f(std::input_iterator_tag);
            void f(std::bidirectional_iterator_tag);
            void f(std::random_access_iterator_tag);
        模拟重载解析最简单的方式就是使用重载解析，就像下面这样:
         
         */
        // construct a set of match() overloads for the types in Types...:
        template<typename... Types>
        struct MatchOverloads;
        
        // basis case: nothing matched:
        template<>
        struct MatchOverloads<> {
            static void match(...);
        };
        
        // recursive case: introduce a new match() overload:
        template<typename T1, typename... Rest>
        struct MatchOverloads<T1, Rest...> : public MatchOverloads<Rest...>
        {
            static T1 match(T1); // introduce overload for T1
            using MatchOverloads<Rest...>::match;// collect overloads from bases
        };
        
        // find the best match for T in Types...
        template<typename T, typename... Types>
        struct BestMatchInSetT {
            using Type = decltype(MatchOverloads<Types...>::match(declval<T> ()));
        };
        
        template<typename T, typename... Types>
        using BestMatchInSet = typename BestMatchInSetT<T, Types...>::Type;
    
        // primary template (intentionally undefined):
        template<typename Iterator,
                typename Tag = BestMatchInSet<
                        typename std::iterator_traits<Iterator> ::iterator_category,
                        std::input_iterator_tag,
                        std::bidirectional_iterator_tag,
                        std::random_access_iterator_tag>>
        class Advance;
    
        // general, linear-time implementation for input iterators:
        template<typename Iterator>
        class Advance<Iterator, std::input_iterator_tag>
        {
        public:
            using DifferenceType = typename
            std::iterator_traits<Iterator>::difference_type;
            void operator() (Iterator& x, DifferenceType n) const
            {
                while (n > 0) {
                    ++x;
                    --n;
                }
            }
        
        };
    
        // bidirectional, linear-time algorithm for bidirectional iterators:
        template<typename Iterator>
        class Advance<Iterator, std::bidirectional_iterator_tag>
        {
        public:
            using DifferenceType =typename std::iterator_traits<Iterator>::difference_type;
            void operator() (Iterator& x, DifferenceType n) const
            {
                if (n > 0)
                    while (n > 0) {
                        ++x; --n;
                    }
            
                else
                    while (n < 0) {
                        --x; ++n;
                    }
            
            }
        };
    
        // bidirectional, constant-time algorithm for random access iterators:
        template<typename Iterator>
        class Advance<Iterator, std::random_access_iterator_tag>
        {
        public:
            using DifferenceType = typename std::iterator_traits<Iterator>::difference_type;
            void operator() (Iterator& x, DifferenceType n) const
            {
                x += n;
            }
        };
    
    }
    
    namespace case5 {
    
    }
}

class Tmp {
    public:
    
    };




int
//main()
main_Class_Specialization_20_4()
{

    return 0;
}