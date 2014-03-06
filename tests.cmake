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
if(NOT SKIPTEST AND NOT CMAKE_CROSSCOMPILING)

find_package(Check)

if(CHECK_FOUND)
    
    macro(thegame_add_test TEST_NAME) 
        add_executable(${TEST_NAME} EXCLUDE_FROM_ALL src/test/c/${TEST_NAME}.c)
        target_link_libraries(${TEST_NAME} THEGAME ${CHECK_LIBRARIES})
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
        set(test_binaries ${test_binaries} ${TEST_NAME})
    endmacro(thegame_add_test)

    enable_testing()
    
    
    thegame_add_test(test_Sets)
    thegame_add_test(test_BeliefFromSensors)
    thegame_add_test(test_BeliefFunctions)
    thegame_add_test(test_BeliefCombination)

    add_custom_target(unit_test ALL  ctest --output-on-failure
        DEPENDS THEGAME ${test_binaries}
        COMMENT "\n\n   ===== TESTS =====   \n\n"
        )    

else(CHECK_FOUND)
    message(WARNING "Check library not found, no test will be executed")
endif(CHECK_FOUND)

endif(NOT SKIPTEST AND NOT CMAKE_CROSSCOMPILING)