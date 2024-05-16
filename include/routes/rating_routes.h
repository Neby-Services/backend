#include <controllers/rating_controller.h>
#include <crow.h>
#include <db/connection_pool.h>
#include <middlewares/verify_jwt.h> 	
#include <db/connection_pool.h>
#include <utils/common.h>
#include <pqxx/pqxx>

void initialize_rating_routes(NebyApp& app);