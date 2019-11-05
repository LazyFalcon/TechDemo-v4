cmake_minimum_required(VERSION 3.5)
project(TechDemo VERSION 1.0.0 LANGUAGES CXX)

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list src/*.hpp)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_compile_definitions(GLM_FORCE_SWIZZLE)
# add_compile_definitions(GLM_SWIZZLE)
add_compile_definitions(GLM_FORCE_RADIANS)
add_compile_definitions(USE_BULLET)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE includes CONFIGURE_DEPENDS src/*.cpp)

add_executable(TechDemo ${includes} ${sources})

target_compile_features(TechDemo PUBLIC cxx_std_17)
set_target_properties(TechDemo PROPERTIES CXX_EXTENSIONS OFF)

include_directories(C:/MinGW/include)
include_directories(C:/MinGW/include/c++/8.2.0)
include_directories(C:/MinGW/lib/gcc/x86_64-w64-mingw32/8.2.0/include)
HEADER_DIRECTORIES(header_list)
include_directories(${header_list})



# https://cliutils.gitlab.io/modern-cmake/
