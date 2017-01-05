include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${LIB_DIR}/rapidxml-1.13) # rapid-xml library
include_directories(${LIB_DIR}/misc_utils)    # Fioretto's miscellanoeus lib

add_executable(test_BE
            CpuTableTest.cpp CpuTableAggregationTest.cpp)

target_link_libraries(test_BE gtest gtest_main)
target_link_libraries(test_BE cpuBE_core)

## Notes: http://geosoft.no/development/unittesting.html