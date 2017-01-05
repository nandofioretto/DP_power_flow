include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${LIB_DIR}/rapidxml-1.13) # rapid-xml library
include_directories(${LIB_DIR}/misc_utils)    # Fioretto's miscellanoeus lib

add_executable(test_constraints
            WSumConstraintTest.cpp XmulCConstraintTest.cpp)

target_link_libraries(test_constraints gtest gtest_main)
target_link_libraries(test_constraints cpuBE_core)

## Notes: http://geosoft.no/development/unittesting.html