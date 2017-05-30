cmake_minimum_required(VERSION 2.8)
project(ccv_ex04)
find_package( OpenCV REQUIRED )
include_directories( include )
add_library(gauss_pyr src/gauss_pyr.cpp)

add_executable(${PROJECT_NAME}_contrasts src/contrasts.cpp)
target_link_libraries(${PROJECT_NAME}_contrasts ${OpenCV_LIBS} gauss_pyr)
