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
    
    ###belief from sensors
    add_executable(test_BeliefFromSensors src/test/c/checkBeliefFromSensors.c)
    target_link_libraries(test_BeliefFromSensors THEGAME ${CHECK_LIBRARIES})
    add_test(NAME test_BeliefFromSensors COMMAND test_BeliefFromSensors)
    
    ###belief functions
    add_executable(test_beliefFunctions src/test/c/checkBeliefFunctions.c)
    target_link_libraries(test_beliefFunctions THEGAME ${CHECK_LIBRARIES})
    add_test(NAME test_beliefFunctions COMMAND test_beliefFunctions)

    set(test_binaries test_BeliefFromSensors)
    add_custom_target(unit_test ALL  ctest --output-on-failure
        DEPENDS THEGAME ${test_binaries}
        COMMENT "\n\n   ===== TESTS =====   \n\n"
        )    

else(CHECK_FOUND)
    message(WARNING "Check library not found, no test will be executed")
endif(CHECK_FOUND)

endif(NOT SKIPTEST AND NOT CMAKE_CROSSCOMPILING)