cmake_minimum_required(VERSION 3.1)

project(contactanglegui)

find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

include_directories(../)

add_executable(contactanglegui
               main.cpp)

target_link_libraries(contactanglegui
                      ${wxWidgets_LIBRARIES}
                      contactangle)