#include <iostream>

#include <gtest/gtest.h>

using namespace std;

int test(int a, int b) {
    return a + b;
}

TEST(test, AddTest) {
    EXPECT_EQ(4, test(2, 2));
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

