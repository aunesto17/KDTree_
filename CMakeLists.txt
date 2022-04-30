# project name
project("KDTree")

# minimun cmake version required
cmake_minimum_required(VERSION 3.15)

# version of C++ that will be used
set(CMAKE_CXX_STANDARD 11)
# set variables for including librarys
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# deactivate cmake extensions that cmake can include
set(CMAKE_CXX_EXTENSIONS OFF)
# set path to include source files
include_directories($(CMAKE_CURRENT_SOURCE_DIR)/src)
# file to be build
add_executable(cmake_test source/main.cpp)