cmake_minimum_required(VERSION 3.11)

project(tasktracker VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Wpedantic)


set (CMAKE_FLAGS "-DINSTALL_GTEST=OFF")

enable_testing()

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)