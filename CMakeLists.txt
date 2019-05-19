cmake_minimum_required(VERSION 3.1)
project(main)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_library(contactangle
            src/contactangle.cpp
            src/geometry.cpp)

add_executable(main
               src/main.cpp
               )
target_link_libraries(contactangle ${OpenCV_LIBS})
target_link_libraries(main contactangle ${OpenCV_LIBS})
