cmake_minimum_required(VERSION 2.8)

include_directories("include")
aux_source_directory("src" COMMON_SRC)
set(CMAKE_CXX_FLAGS  "-std=c++11 -m32 -O3 ${CMAKE_CXX_FLAGS}")

add_library(common STATIC ${COMMON_SRC})