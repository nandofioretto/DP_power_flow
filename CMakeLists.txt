cmake_minimum_required(VERSION 3.6)
project(CpuBE)

set(CpuBE_MAJOR_VERSION 0)
set(CpuBE_MINOR_VERSION 1)
set(CpuBE_PATCH_VERSION 0)
set(VERSION  ${CpuBE_MAJOR_VERSION}.${CpuBE_MINOR_VERSION}.${CpuBE_PATCH_VERSION})

add_definitions(-std=c++14)

# add the binary tree to the search path for include files
# so that we will find PowerToolsConfig.h.
include_directories("${PROJECT_BINARY_DIR}")

# Build options
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


#set(SOURCE_DIR src/)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib/)
set(SOURCE_FILES main.cpp)

add_executable(cpuBE ${SOURCE_FILES})

include_directories(${LIB_DIR}/rapidxml-1.13) # rapid-xml library
include_directories(${LIB_DIR}/misc_utils)    # Fioretto's miscellanoeus lib
include_directories(src)

add_subdirectory(src)
#add_subdirectory(tests)

target_link_libraries(cpuBE cpuBE_core)