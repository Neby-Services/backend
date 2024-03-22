cmake_minimum_required(VERSION 3.18)

project(backend)

set(CMAKE_CXX_STANDARD 20)

# Agrega las opciones de compilación
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

# Encuentra las bibliotecas necesarias
add_subdirectory(extern/cpr)
add_subdirectory(extern/googletest)
add_subdirectory(extern/json)

# Si bcrypt está instalado en un lugar no estándar, puedes usar find_path para encontrar la carpeta de inclusión
# find_path(BCRYPT_INCLUDE_DIR bcrypt/bcrypt.h)

# Establece las fuentes de tu proyecto
file(GLOB_RECURSE SOURCES "tests/*.cpp")

# Crea el ejecutable
add_executable(test ${SOURCES})

# Agrega las carpetas de inclusión
target_include_directories(test PUBLIC include)

# Enlaza las bibliotecas necesarias
target_link_libraries(test ${PQXX_LIB} ${PQ_LIB} gtest gtest_main cpr nlohmann_json::nlohmann_json)

# Si bcrypt está en un lugar no estándar, también agrega el directorio de inclusión
# target_include_directories(backend PUBLIC ${BCRYPT_INCLUDE_DIR})
