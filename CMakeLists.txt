cmake_minimum_required(VERSION 2.6)

project(THEGAME C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

file(GLOB_RECURSE src_thegame src/main/c/*)

include_directories(src/main/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")


set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# ==== declare library ====

add_library(THEGAME SHARED ${src_thegame})
	
target_link_libraries(THEGAME rt m)

add_library(THEGAME-static STATIC ${src_thegame})
	
target_link_libraries(THEGAME-static rt m)


set_target_properties(
    THEGAME THEGAME-static
    PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -Werror -pedantic"
    OUTPUT_NAME THEGAME
)
    

# ====== testing =======


include(tests.cmake)