#include <gtest/gtest.h>
#include <cpr/cpr.h>

int sum(int a, int b) {
    return a + b;
}

TEST(test1, PositiveNumbers) {
    EXPECT_EQ(sum(2, 3), 5);
}

TEST(IntegrationTest, HttpRequest) {
    auto response = cpr::Get(cpr::Url{"http://backend:3000/api/users"});


    EXPECT_EQ(response.status_code, 200);

}
