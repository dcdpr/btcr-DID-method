file(GLOB SRCS *.cpp)

add_executable(txref ${SRCS})

install(TARGETS txref DESTINATION bin)