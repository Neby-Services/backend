#include <gtest/gtest.h>

#include <iostream>
// Incluye tus archivos de prueba aquí
// ...



TEST(ddd, a) {
    EXPECT_EQ(5+5, 5);
}


int main(int argc, char** argv) {
    // Inicializa Google Test
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}