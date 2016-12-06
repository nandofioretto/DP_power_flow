include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${LIB_DIR}/rapidxml-1.13) # rapid-xml library
include_directories(${LIB_DIR}/misc_utils)    # Fioretto's miscellanoeus lib

add_executable(run_core_tests
            WSumConstraintTest.cpp)

target_link_libraries(run_core_tests gtest gtest_main)
target_link_libraries(run_core_tests cpuBE_core)

## Notes: http://geosoft.no/development/unittesting.html