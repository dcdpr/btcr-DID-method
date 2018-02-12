file(GLOB SRCS *.cpp)

add_executable(testbech32 ${SRCS})

target_link_libraries(testbech32 libbech32 gtest gmock)

add_test(NAME testbech32
         COMMAND testbech32)
