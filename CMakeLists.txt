cmake_minimum_required(VERSION 3.5)
project(TechDemo VERSION 1.0.0 LANGUAGES CXX)

add_executable(TechDemo, src/App/main.cpp)
target_compile_features(myTarget PUBLIC cxx_std_17)
set_target_properties(myTarget PROPERTIES CXX_EXTENSIONS OFF)


# https://cliutils.gitlab.io/modern-cmake/
