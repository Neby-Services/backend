#pragma once

#include <crow.h>
#include <format>
#include <memory>
#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <vector>

// ** custom includes
#include <models/service_model.h>
#include <models/user_model.h>
#include <models/rating_model.h>
#include <utils/common.h>
#include <utils/errors.h>
#include <utils/utils.h>
#include <utils/validations.h>
// ** ---------------------------------------------

class RatingController {
    public:

    static void create_rating(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& service_id);
    static void update_rating(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& rating_id);
    static void get_user_ratings(pqxx::connection& db, crow::response& res, const std::string& user_id);
};