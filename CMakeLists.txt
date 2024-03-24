cmake_minimum_required(VERSION 3.18)

project(backend)

set(CMAKE_CXX_STANDARD 20)

# Agrega las opciones de compilación
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Encuentra las bibliotecas necesarias
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)
find_library(BCRYPT_LIB bcrypt)

include(FetchContent)
fetchcontent_declare(jwt-cpp 
	GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
	GIT_TAG 08bcf77a687fb06e34138e9e9fa12a4ecbe12332 # v0.7.0 release
)
set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "disable building examples" FORCE)
fetchcontent_makeavailable(jwt-cpp)
# Si bcrypt está instalado en un lugar no estándar, puedes usar find_path para encontrar la carpeta de inclusión
# find_path(BCRYPT_INCLUDE_DIR bcrypt/bcrypt.h)

# Establece las fuentes de tu proyecto
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Crea el ejecutable
add_executable(backend ${SOURCES})

# Agrega las carpetas de inclusión
target_include_directories(backend PUBLIC include)

# Enlaza las bibliotecas necesarias
target_link_libraries(backend ${PQXX_LIB} ${PQ_LIB} ${BCRYPT_LIB} jwt-cpp::jwt-cpp)

# Si bcrypt está en un lugar no estándar, también agrega el directorio de inclusión
# target_include_directories(backend PUBLIC ${BCRYPT_INCLUDE_DIR})
