cmake_minimum_required(VERSION 3.6)
project(txref)

set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS NO)

set(DCD_CXX_FLAGS "-Weverything -Wno-c++98-compat")

add_subdirectory(libbech32)
add_subdirectory(libtxref)
add_subdirectory(test)

