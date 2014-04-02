cmake_minimum_required(VERSION 2.6)

project(THEGAME C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

file(GLOB_RECURSE src_thegame src/main/c/core/*.c)

file(GLOB_RECURSE src_thegame_xml src/main/c/xml/*.c)

include_directories(src/main/include/public src/main/include/private)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(thegame_compile_flags "-Wall -Wextra -Werror -pedantic")

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
    COMPILE_FLAGS ${thegame_compile_flags}
    OUTPUT_NAME THEGAME
)



find_package(LibXml2)

if(LIBXML2_FOUND)

    add_library(THEGAME-xml SHARED ${src_thegame_xml})
    include_directories(${LIBXML2_INCLUDE_DIR})
    target_link_libraries(THEGAME-xml THEGAME ${LIBXML2_LIBRARIES})
    set_target_properties(
    
    THEGAME-xml
    PROPERTIES
    COMPILE_FLAGS ${thegame_compile_flags}
    )
    
else(LIBXML2_FOUND)
    message(WARNING "libxml2 not found, cannot compile THEGAME-xml.")
endif(LIBXML2_FOUND)

# ====== documentation ======

find_package(Doxygen)

if(DOXYGEN_FOUND)
    configure_file( doc/Doxyfile.in generatedDoc/Doxyfile @ONLY)
    add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/generatedDoc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generatedDoc
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES generatedDoc)
endif(DOXYGEN_FOUND)

# ====== testing =======


include(tests.cmake)