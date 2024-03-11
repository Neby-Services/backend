cmake_minimum_required(VERSION 3.18)

project(backend)

set(CMAKE_CXX_STANDARD 20)

# Agrega las opciones de compilación
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Encuentra las bibliotecas necesarias
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)
find_library(BCRYPT_LIB bcrypt)

# Si bcrypt está instalado en un lugar no estándar, puedes usar find_path para encontrar la carpeta de inclusión
# find_path(BCRYPT_INCLUDE_DIR bcrypt/bcrypt.h)

# Establece las fuentes de tu proyecto
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Crea el ejecutable
add_executable(backend ${SOURCES})

# Agrega las carpetas de inclusión
target_include_directories(backend PUBLIC include)

# Enlaza las bibliotecas necesarias
target_link_libraries(backend ${PQXX_LIB} ${PQ_LIB} ${BCRYPT_LIB})

# Si bcrypt está en un lugar no estándar, también agrega el directorio de inclusión
# target_include_directories(backend PUBLIC ${BCRYPT_INCLUDE_DIR})
