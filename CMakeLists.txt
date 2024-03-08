cmake_minimum_required(VERSION 3.10)
project(backend)

set(CMAKE_CXX_STANDARD 20)

# Add executable
add_executable(backend main.cpp
routes/test_routes.cpp
controllers/test_controller.cpp
)

# Include Crow header files
target_include_directories(backend PUBLIC ${CROW_INCLUDE_DIRS})
