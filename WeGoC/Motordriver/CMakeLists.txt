cmake_minimum_required(VERSION 3.0)
project(my_project)

add_executable(my_executable main.c
        lg/lgpio.h)

target_link_libraries(my_executable lgpio)