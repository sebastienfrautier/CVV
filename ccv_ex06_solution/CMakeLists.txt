cmake_minimum_required(VERSION 2.8)
project(ccv_ex06)
find_package( OpenCV REQUIRED )

add_executable(${PROJECT_NAME}_fusion src/fusion.cpp)
target_link_libraries(${PROJECT_NAME}_fusion ${OpenCV_LIBS})