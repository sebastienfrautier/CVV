cmake_minimum_required(VERSION 2.8)
project(ccv_ex05)
find_package( OpenCV REQUIRED )
include_directories( include )
add_library(pyramids src/gauss_pyr.cpp src/laplacian_pyr.cpp src/oriented_pyr.cpp)

add_executable(${PROJECT_NAME}_gabor src/gabor.cpp)
target_link_libraries(${PROJECT_NAME}_gabor ${OpenCV_LIBS})

add_executable(${PROJECT_NAME}_orientation_test src/orientation_test.cpp)
target_link_libraries(${PROJECT_NAME}_orientation_test ${OpenCV_LIBS} pyramids)