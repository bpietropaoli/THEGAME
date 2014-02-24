cmake_minimum_required(VERSION 2.6)

project(thegame)

file(GLOB_RECURSE src_thegame src/main/c/*)

include_directories(src/main/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic")

set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# ==== declare library ====

add_library(THEGAME SHARED ${src_thegame})
	
target_link_libraries(THEGAME rt m)

add_library(THEGAME-static STATIC ${src_thegame})
	
target_link_libraries(THEGAME-static rt m)

# ====== testing =======

enable_testing()

file(GLOB_RECURSE src_thegame_test src/test/c/*)

add_executable(
	test-thegame
	
	${src_thegame_test}
)

include_directories(src/test/include)

target_link_libraries(test-thegame THEGAME)

add_test(test-thegame test-thegame)

