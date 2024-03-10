cmake_minimum_required(VERSION 3.18)

project(backend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpqxx -lpq")
set(PQXX /usr/local/include/pqxx)

find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(backend ${SOURCES})

target_link_libraries(backend ${PQXX_LIB} ${PQ_LIB})
