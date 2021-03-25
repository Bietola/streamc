# initialize project
cmake_minimum_required(VERSION 3.11)
project(streamc VERSION 1.0.0 LANGUAGES CXX C)

# CMAKE modules
include(FetchContent)

add_executable(streamc
    # CPP sources
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/special_keycodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/keymap.cpp

    # C sources
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rosetta_utf8.c
)

# export compilation database for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# include directories
target_include_directories(streamc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# compiler properties
target_compile_options(streamc PRIVATE -Werror)
target_compile_features(streamc PRIVATE cxx_std_17 c_std_99)

# Include sources for header only for `libfmt`
target_sources(streamc PRIVATE include/fmt/format.cc)

# Fetch content for `nlohmann_json`
FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Link libraries
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(streamc PRIVATE
    uiohook
    nlohmann_json::nlohmann_json)
