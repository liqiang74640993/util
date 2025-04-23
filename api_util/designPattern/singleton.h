#ifndef SINGLETON_H
#define SINGLETON_H

#include <iostream>

// 单例模板类(最好的方式:安全，延迟加载)
template <typename T>
class Singleton {
public:
    // 静态成员函数，用于获取单例实例
    static T& getInstance() {
        //C++（C++11 及以后）标准下，static T instance; 这种在静态成员函数里定义局部静态变量的方式用于实现单例模式是线程安全的，不会存在创建多个实例的问题
        //如果一个线程正在初始化局部静态变量，其他线程尝试初始化该变量时会被阻塞，直到初始化完成。这就保证了局部静态变量只会被初始化一次，进而确保单例模式中只有一个实例被创建。
        static T instance;
        return instance;
    }

protected:
    // 保护构造函数，防止外部直接实例化
    Singleton() = default;
    // 禁止拷贝构造
    Singleton(const Singleton&) = delete;
    // 禁止赋值操作
    Singleton& operator=(const Singleton&) = delete;
};

// 示例类，继承自单例模板类
class MyClass : public Singleton<MyClass> {

    // 为了让单例模板类可以访问私有构造函数
    friend class Singleton<MyClass>;

private:
    // 私有构造函数，防止外部实例化
    MyClass() = default;

public:
    void doSomething() {
        std::cout << "MyClass is doing something." << std::endl;
    }
};


//饿汉式单例模式
//“饿汉式” 单例模式就像一个 “饥饿” 的人，在类加载的时候就立即创建单例实例，不管之后是否会使用这个实例。
//这意味着实例的创建是在程序启动阶段完成的，所以实例的创建过程是线程安全的
//优点：实现简单，线程安全，无需额外的同步措施。
//缺点：无论该单例是否会被使用，实例都会在程序启动时创建，可能会造成资源的浪费。

class Singleton1 {
private:
    // 构造函数私有化，防止外部实例化
    Singleton1() {}
    // 拷贝构造函数和赋值运算符私有化，防止拷贝和赋值
    Singleton1(const Singleton1&) = delete;
    Singleton1& operator=(const Singleton1&) = delete;
    // 静态成员变量，保存唯一实例
    static Singleton1 instance;

public:
    // 静态成员函数，用于获取唯一实例
    static Singleton1& getInstance() {
        return instance;
    }
};

// 初始化静态成员变量
Singleton1 Singleton1::instance;

#include <mutex>


//线程安全的懒汉式
//优点：实现了延迟加载，避免了不必要的资源浪费。
//缺点：实现相对复杂，尤其是在多线程环境下需要考虑线程安全问题，可能会引入额外的同步开销。

class Singleton2 {
private:
    Singleton2() {}
    Singleton2(const Singleton2&) = delete;
    Singleton2& operator=(const Singleton2&) = delete;
    static Singleton2* instance;
    static std::mutex mutex_;

public:
    static Singleton2* getInstance() {
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            //存在2个线程同时执行到std::lock_guard<std::mutex> lock(mutex_)，
            //即都进入了外层的null判断，这样就可能创建对个对象，因此需要在枷锁后再次判断
            if (instance == nullptr) { //
                instance = new Singleton2();
            }
        }
        return instance;
    }
};

Singleton2* Singleton2::instance = nullptr;
std::mutex Singleton2::mutex_;



#endif // SINGLETON_H
