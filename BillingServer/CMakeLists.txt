cmake_minimum_required(VERSION 2.8)

include_directories("include" "${PROJECT_SOURCE_DIR}/Common/include")
include_directories("${PROJECT_SOURCE_DIR}/mysql/include")
aux_source_directory("src" BILLING_SRC)

set(CMAKE_CXX_FLAGS "-std=c++11 -m32 -g0 -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
link_directories("${PROJECT_SOURCE_DIR}/lib")

link_libraries("pthread" "dl" "rt")

add_library(billingserver STATIC ${BILLING_SRC})
target_link_libraries(billingserver common mysqlclient)
