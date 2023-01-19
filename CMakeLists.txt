cmake_minimum_required(VERSION 3.16)
project(lab2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopencilk")
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -O0")

add_executable(lab2
        main.cpp
        graph.cpp
        bfs_seq.cpp
        bfs_par.cpp)
