file(GLOB SRCS *.cpp)

add_library(libbech32 STATIC ${SRCS})

# include directories exported by this library
target_include_directories(libbech32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})