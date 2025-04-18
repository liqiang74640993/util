#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

using namespace std;

int Factorial(int n)
{
    int result = 1;
    for (int i = 1; i <= n; i++)
    {
        result *= i;
    }

    return result;
}

TEST(Factorial, Empty)
{
    EXPECT_EQ(1, Factorial(1));
}



int main(int argc, char *argv[])
{
    //::testing::InitGoogleTest(&argc, argv); //googleTest框架
    //RUN_ALL_TESTS();//googleTest框架
    return 0;
}
