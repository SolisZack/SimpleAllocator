cmake_minimum_required(VERSION 3.19)
project(Allocator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

add_executable(Allocator
        main.cpp
        memory_pool.h
        memory_pool.tcc)


