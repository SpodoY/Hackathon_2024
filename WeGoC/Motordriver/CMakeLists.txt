cmake_minimum_required(VERSION 3.0)
project(my_project)

include_directories(lg)

add_executable(gpioTest main.c)
target_link_libraries(gpioTest lgpio)