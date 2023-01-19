cmake_minimum_required(VERSION 3.16)
project(lab2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#set(CMAKE_THREAD_LIBS_INIT "-lpthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
#set(CMAKE_HAVE_THREADS_LIBRARY 1)
#set(CMAKE_USE_WIN32_THREADS_INIT 0)
#set(CMAKE_USE_PTHREADS_INIT 1)
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#set(CPLUS_INCLUDE_PATH "/opt/opencilk/include")

add_executable(lab2
        main.cpp
        graph.cpp
        bfs_seq.cpp
        bfs_par.cpp)

#find_package(TBB)
#target_link_libraries(lab2 TBB::tbb)