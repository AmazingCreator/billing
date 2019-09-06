cmake_minimum_required(VERSION 2.8)

project(billing)

set(LIBRARY_OUTPUT_PATH  "${PROJECT_SOURCE_DIR}/lib")

add_definitions("-DDBUG_OFF")
include_directories("${PROJECT_SOURCE_DIR}/BillingServer/include")
include_directories("${PROJECT_SOURCE_DIR}/Common/include")
include_directories("${PROJECT_SOURCE_DIR}/ResServer/include")

add_subdirectory("Common")
add_subdirectory("BillingServer")
add_subdirectory("ResServer")
add_subdirectory("mysql")

set(CMAKE_BUILD_TYPE "MinSizeRel")
set(CMAKE_CXX_FLAGS "-std=c++11 -static-libstdc++ -m32 -g0 -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")

link_directories("${PROJECT_SOURCE_DIR}/mysql/lib")
link_directories("${PROJECT_SOURCE_DIR}/lib")

link_libraries("pthread" "dl")
add_executable(billing main.cpp)
target_link_libraries(billing common billingserver resserver)