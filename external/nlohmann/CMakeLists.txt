cmake_minimum_required(VERSION 3.8)

project(nlohmann-json
    VERSION 3.7.3
    LANGUAGES CXX)

add_library(nlohmann-json INTERFACE)

target_include_directories(nlohmann-json INTERFACE
        "${PROJECT_SOURCE_DIR}"
        )
