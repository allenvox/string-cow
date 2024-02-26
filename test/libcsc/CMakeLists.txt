include(GoogleTest)

add_executable(
        string.test
        string.cpp
)
set_compile_options(string.test)
target_include_directories(
        string.test
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(
        string.test
        GTest::gtest_main
        string
)
gtest_discover_tests(string.test)