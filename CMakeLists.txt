cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(snfd C)

# location of additional cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(ExternalProject)


ExternalProject_Add(
    nand-flash-emulator
    GIT_REPOSITORY https://github.com/inclooder/nand-flash-emulator.git 
    INSTALL_DIR ${PROJECT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

set(UNITY_ROOT external/Unity CACHE STRING "Unity source root")
message(STATUS "UNITY_ROOT set to ${UNITY_ROOT}")

# location of header files
include_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/${UNITY_ROOT}
    ${PROJECT_SOURCE_DIR}/${UNITY_ROOT}/src
    ${PROJECT_BINARY_DIR}/nfe/include
)

file(GLOB SNFD_SOURCES "src/*.c")

add_library(snfd ${SNFD_SOURCES})
add_dependencies(snfd nand-flash-emulator)


# target to update git submodules
add_custom_target(
    update_submodules
    COMMAND git submodule init
    COMMAND git submodule update
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)


link_directories(
    ${LINK_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/nfe/lib
)

# unity sources
set(UNITY_SOURCES
    ${PROJECT_SOURCE_DIR}/${UNITY_ROOT}/src/unity.c
)

# mark these files as generated (they may not be present at configure time)
foreach(_source ${UNITY_SOURCES})
    set_source_files_properties(${_source} PROPERTIES GENERATED 1)
endforeach()

add_library(unity ${UNITY_SOURCES})

# update git submodules before building 
add_dependencies(unity update_submodules)

file(GLOB UNIT_TEST_SOURCES "test/*")

# unit test executable
add_executable(
    unit_tests
    ${UNIT_TEST_SOURCES}
)

# unit test executable depends on unity
add_dependencies(unit_tests unity)

# link unit test executable against unity
target_link_libraries(
    unit_tests
    nfe
    unity
    snfd
)


# activate ctest
include(CTest)
enable_testing()

# define a test
add_test(unit ${PROJECT_BINARY_DIR}/unit_tests)

# make install and cpack
include(install_target)
