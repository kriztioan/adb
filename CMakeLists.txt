cmake_minimum_required (VERSION 3.2.0)

project (AdB)

set(CMAKE_CXX_COMPILER /usr/local/bin/g++-12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-flto -Wall -Wextra -O3 -march=native -fno-exceptions")

find_package(PkgConfig REQUIRED)
pkg_check_modules (JEMALLOC jemalloc)
pkg_search_module(JEMALLOC REQUIRED jemalloc)
include_directories(${JEMALLOC_INCLUDE_DIRS})
link_directories(${JEMALLOC_LIBRARY_DIRS})

set(OPENSSL_ROOT_DIR /usr/local/Cellar/openssl@3/3.0.5/)
find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(. ./include /usr/local/include)

file(GLOB SRC
     "src/*.cpp"
)

add_executable(index.cgi ${SRC})
target_link_libraries(index.cgi ${JEMALLOC_LIBRARIES} ${OPENSSL_LIBRARIES})

add_custom_command(TARGET index.cgi PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/web ${CMAKE_BINARY_DIR}/adb)
add_custom_command(TARGET index.cgi POST_BUILD COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/index.cgi ${CMAKE_BINARY_DIR}/adb/index.cgi)
