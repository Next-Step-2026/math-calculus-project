cmake_minimum_required(VERSION 3.15)
project(mathlab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(_mathcore
    cpp/bindings.cpp
    cpp/src/series.cpp
    cpp/src/integration.cpp
    cpp/src/taylor.cpp
)

target_include_directories(_mathcore PRIVATE cpp/include)

install(TARGETS _mathcore DESTINATION mathlab)

enable_testing()
add_executable(maximum_test cpp/src/maximum.cpp)
target_include_directories(maximum_test PRIVATE cpp/include)
add_test(NAME MaximumContractsTest COMMAND maximum_test)

