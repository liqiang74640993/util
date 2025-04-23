#include  <gtest/gtest.h>
#include  "singleton.h"
#include "templatemethod.h"

TEST(DesignPatternTest, Singleton)
{
   MyClass& singleton = MyClass::getInstance();
   singleton.doSomething();

   Singleton1 &s1 = Singleton1::getInstance();
   Singleton1 &s1_1 = Singleton1::getInstance();

   EXPECT_EQ(&s1, &s1_1); //compare address

   Singleton2 *s2 = Singleton2::getInstance();
   Singleton2 *s2_1 = Singleton2::getInstance();

   EXPECT_EQ(s2, s2_1);//compare address
}

TEST(DesignPatternTest, TemplateMethod)
{
   //创建具体子类对象
   ConcreteClass1 concrete1;
   ConcreteClass2 concrete2;

   // 调用模板方法
   std::cout << "ConcreteClass1 execution:" << std::endl;
   concrete1.templateMethod();

   std::cout << "\nConcreteClass2 execution:" << std::endl;
   concrete2.templateMethod();
}
