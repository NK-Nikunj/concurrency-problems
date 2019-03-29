cmake_minimum_required(VERSION 3.3.2)

project(my_hpx_project CXX)

find_package(HPX REQUIRED)
add_hpx_executable(readers_writers
    SOURCES readers_writers.cpp
    COMPONENT_DEPENDENCIES iostreams)