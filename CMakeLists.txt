cmake_minimum_required(VERSION 3.9)
project(txref)

set(CMAKE_CXX_STANDARD 11)

#add_subdirectory(src)
add_subdirectory(libbech32)
add_subdirectory(libtxref)
add_subdirectory(test)

