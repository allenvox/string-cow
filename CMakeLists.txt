cmake_minimum_required(VERSION 3.22)

project(cscstring)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(CompileOptions)

find_program(CLANG_TIDY_EXE NAMES clang-tidy-14 clang-tidy)
if (NOT CLANG_TIDY_EXE)
    message(WARNING "clang-tidy not found")
else()
    execute_process(
            COMMAND ${CLANG_TIDY_EXE} --version
            OUTPUT_VARIABLE CLANG_TIDY_VERSION)
    message("clang-tidy found:\n" ${CLANG_TIDY_VERSION})
endif()

enable_testing()

add_subdirectory(external)
add_subdirectory(test)
