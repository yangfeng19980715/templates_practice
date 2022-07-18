cmake_minimum_required(VERSION 3.22)
project(templates_practice)

set(CMAKE_CXX_STANDARD 14)

add_executable(templates_practice headers.h
        chapter4/variadic_templates_4.1.cpp
        )
