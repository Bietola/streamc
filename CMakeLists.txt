# initialize project
cmake_minimum_required(VERSION 3.5)
project(Proj VERSION 1.0.0 LANGUAGES C)

# load packages
#TODO: no packages for now

# create target executable
add_executable(Proj src/main.c)

# include directories
target_include_directories(Proj
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# compiler properties
target_compile_options(Proj PRIVATE -Werror)

# link libraries
#TODO: no libraries for now
