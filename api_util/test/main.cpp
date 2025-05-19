#include <iostream>
#include <gtest/gtest.h>

int temp(char *s){
    printf("%s\n",s);
    return 0;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) = "ProcessIPCTest.*";
    return RUN_ALL_TESTS();
}
