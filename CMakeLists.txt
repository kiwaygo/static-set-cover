cmake_minimum_required(VERSION 3.12)

project(static-set-cover)

set(CMAKE_CXX_STANDARD 17)

add_library(static-set-cover INTERFACE)
target_include_directories(static-set-cover INTERFACE include)

find_package(GTest)
if (${GTest_FOUND})
    include(CTest)
    add_subdirectory(test)
endif()
