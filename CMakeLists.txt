cmake_minimum_required(VERSION 3.1)
project(contactangle)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_library(contactangle
            src/contactangle.cpp
            src/geometry.cpp)

add_executable(main
               src/main.cpp)

target_link_libraries(contactangle ${OpenCV_LIBS})
target_link_libraries(main contactangle ${OpenCV_LIBS})

option(BUILD_GUI "Build the GUI" OFF)
if(BUILD_GUI)
    add_subdirectory(src/gui)
endif()

option(BUILD_PY_WRAP "Build the python library" OFF)
if(BUILD_PY_WRAP)
    add_subdirectory(pyext)
endif()


option(PACKAGE_TESTS "Build the tests" OFF)
if(PACKAGE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
