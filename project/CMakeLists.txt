cmake_minimum_required(VERSION 2.8)
project(saliency_project)
find_package( OpenCV REQUIRED )
include_directories( include )
add_library(pyramids src/gauss_pyr.cpp src/laplacian_pyr.cpp src/oriented_pyr.cpp)

add_executable(${PROJECT_NAME}_main src/main.cpp)
target_link_libraries(${PROJECT_NAME}_main ${OpenCV_LIBS} pyramids)
