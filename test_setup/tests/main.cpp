#include <iostream>
#include <gtest/gtest.h>

int main(int argc, char** argv) {
    // Inicializa Google Test
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}