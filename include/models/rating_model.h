#pragma once

#include <utils/common.h>
#include <utils/errors.h>
#include <format>
#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class RatingModel {
    private:
    std::string _id;
    std::string _service_id;
    int _rating;

    public:

    RatingModel(std::string id, std::string service_id, int rating);

    std::string get_id() const;
    std::string get_service_id() const;
    int get_rating() const;

    static std::unique_ptr<RatingModel> create_rating(pqxx::connection& db, const std::string& service_id, const int& rating, bool throw_when_null);
    static std::unique_ptr<RatingModel> update_rating(pqxx::connection& db, const std::string& rating_id, const int& rating);
    static std::vector<std::unique_ptr<RatingModel>> get_rating_by_user_id(pqxx::connection& db, const std::string& user_id);
    static std::unique_ptr<RatingModel> get_rating_by_service_id(pqxx::connection& db, const std::string& service_id);
    static std::unique_ptr<RatingModel> get_rating_by_id(pqxx::connection& db, const std::string& rating_id);
};