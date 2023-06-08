cmake_minimum_required(VERSION 3.14)

project(Serialize LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (SOURCE
     serialize.cpp
     main.cpp)

add_executable (Serialize
                main.cpp)
