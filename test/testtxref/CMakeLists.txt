file(GLOB SRCS *.cpp)

add_executable(testtxref ${SRCS})

target_link_libraries(testtxref libtxref libbech32 gtest gmock)

add_test(NAME testtxref
         COMMAND testtxref)
