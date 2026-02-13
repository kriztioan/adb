cmake_minimum_required (VERSION 4.0.1)

project (AdB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-flto -Wall -Wextra -O3 -march=native -mtune=native -fno-exceptions")

find_package(PkgConfig REQUIRED)
pkg_search_module(BTPARSE REQUIRED btparse)
pkg_search_module(JEMALLOC REQUIRED jemalloc)
pkg_search_module(OPENSSL REQUIRED openssl)
pkg_search_module(RAPIDJSON REQUIRED rapidjson)

file(GLOB SRC
     "src/*.cpp"
)

add_executable(index.cgi ${SRC})

target_include_directories(index.cgi PUBLIC . ./include ${JEMALLOC_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIRS} ${RAPIDJSON_INCLUDE_DIRS} ${BTPARSE_INCLUDE_DIRS})

target_link_libraries(index.cgi PUBLIC ${JEMALLOC_LINK_LIBRARIES} ${OPENSSL_LINK_LIBRARIES} ${BTPARSE_LINK_LIBRARIES})

add_custom_command(TARGET index.cgi PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/web ${CMAKE_BINARY_DIR}/adb)
add_custom_command(TARGET index.cgi POST_BUILD COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/index.cgi ${CMAKE_BINARY_DIR}/adb/index.cgi)
