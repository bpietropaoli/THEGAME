cmake_minimum_required(VERSION 2.6)

project(THEGAME C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

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


## original test :

file(GLOB_RECURSE src_thegame_test src/test/c/main.c src/test/c/Tests.c )

add_executable(
	test-thegame
	${src_thegame_test}
)

file(COPY src/test/resources/data DESTINATION .)

include_directories(src/test/include)

target_link_libraries(test-thegame THEGAME)

## unit testing
if(NOT CMAKE_CROSSCOMPILING)

find_package(Check)

if(CHECK_FOUND)
    enable_testing()
    
    add_executable(test_BeliefFromSensors src/test/c/checkBeliefFromSensors.c)
    
    target_link_libraries(test_BeliefFromSensors THEGAME ${CHECK_LIBRARIES})
    
    add_test(NAME test_BeliefFromSensors COMMAND test_BeliefFromSensors)
    
else(CHECK_FOUND)
    message(WARNING "Check library not found, no test will be executed")
endif(CHECK_FOUND)

endif(NOT CMAKE_CROSSCOMPILING)