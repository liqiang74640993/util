#ifndef TEMPLATEMETHOD_H
#define TEMPLATEMETHOD_H

#include <iostream>

//模板方法设计模式（Template Method Design Pattern）是一种行为设计模式，它在一个抽象类中定义了一个算法的骨架，
//将算法中的某些步骤延迟到子类中实现。这样，子类可以在不改变算法整体结构的情况下，对算法的特定步骤进行个性化实现。
// 抽象类
class AbstractClass {
public:
    // 模板方法，定义算法的骨架
    void templateMethod() {
        step1();
        step2();
        step3();
    }

    // 步骤 1，具体方法，提供默认实现
    void step1() {
        std::cout << "AbstractClass: Step 1" << std::endl;
    }

    // 步骤 2，抽象方法，由子类实现
    virtual void step2() = 0;

    // 步骤 3，具体方法，提供默认实现
    void step3() {
        std::cout << "AbstractClass: Step 3" << std::endl;
    }

    virtual ~AbstractClass() {}
};

// 具体子类 1
class ConcreteClass1 : public AbstractClass {
public:
    // 实现步骤 2
    void step2() override {
        std::cout << "ConcreteClass1: Step 2" << std::endl;
    }
};

// 具体子类 2
class ConcreteClass2 : public AbstractClass {
public:
    // 实现步骤 2
    void step2() override {
        std::cout << "ConcreteClass2: Step 2" << std::endl;
    }
};
#endif // TEMPLATEMETHOD_H
