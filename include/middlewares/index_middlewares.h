#include <crow.h>
#include <middlewares/handle_achievements.h>
#include <middlewares/verify_jwt.h>

using NebyApp = crow::App<VerifyJWT, HandleAchievements>;
