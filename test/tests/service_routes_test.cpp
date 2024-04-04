#include <cpr/cpr.h>
#include <gtest/gtest.h>

#include <cstdlib>	// Para std::getenv
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include "common.h"

class GetAllServices : public ::testing::Test {
	protected:
	void SetUp() override {
		// Code here will be called immediately after the constructor (right
		// before each test).
	}
	/* 	void TearDown() override {
			clean_user_table();
		} */
};

// Define your test cases within this class or other test fixture classes

TEST_F(GetAllServices, first) {
	// create_services();
	EXPECT_EQ(2, 2);
}
