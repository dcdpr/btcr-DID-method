file(GLOB SRCS *.cpp)

add_library(libtxref STATIC ${SRCS})

# include directories exported by this library
target_include_directories(libtxref PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
