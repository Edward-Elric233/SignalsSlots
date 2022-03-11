模仿 Boost.Signals 接口实现的简单易用 C++ Signal/Slot 库，支持动态连接和删除槽函数
- 利用现代 C++ 的可变参模板、偏特化以及完美转发等语法特性
- 采用 Observer 设计模式，使用互斥锁和智能指针配合的 Copy On Write 保证线程安全
- 借鉴陈硕大神的实现，发现一个万能引用的 Bug ，并提交 PR。（https://github.com/chenshuo/recipes/pull/34）