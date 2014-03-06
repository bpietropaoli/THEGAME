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
    enable_testing()
    
    ###sets
    add_executable(test_Sets EXCLUDE_FROM_ALL src/test/c/checkSets.c)
    target_link_libraries(test_Sets THEGAME ${CHECK_LIBRARIES})
    add_test(NAME test_Sets COMMAND test_Sets)
    
    ###belief from sensors
    add_executable(test_BeliefFromSensors EXCLUDE_FROM_ALL src/test/c/checkBeliefFromSensors.c)
    target_link_libraries(test_BeliefFromSensors THEGAME ${CHECK_LIBRARIES})
    add_test(NAME test_BeliefFromSensors COMMAND test_BeliefFromSensors)
    
    ###belief functions
    add_executable(test_beliefFunctions EXCLUDE_FROM_ALL src/test/c/checkBeliefFunctions.c)
    target_link_libraries(test_beliefFunctions THEGAME ${CHECK_LIBRARIES})
    add_test(NAME test_beliefFunctions COMMAND test_beliefFunctions)
    
    ###belief combinations
    add_executable(test_beliefCombination EXCLUDE_FROM_ALL src/test/c/checkBeliefCombination.c)
    target_link_libraries(test_beliefCombination THEGAME ${CHECK_LIBRARIES})
    add_test(NAME test_beliefCombination COMMAND test_beliefCombination)

    set(test_binaries test_Sets test_beliefFunctions test_beliefCombination test_BeliefFromSensors )
    add_custom_target(unit_test ALL  ctest --output-on-failure
        DEPENDS THEGAME ${test_binaries}
        COMMENT "\n\n   ===== TESTS =====   \n\n"
        )    

else(CHECK_FOUND)
    message(WARNING "Check library not found, no test will be executed")
endif(CHECK_FOUND)

endif(NOT SKIPTEST AND NOT CMAKE_CROSSCOMPILING)