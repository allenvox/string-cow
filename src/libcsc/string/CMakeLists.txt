set(target_name string)
add_library(${target_name} STATIC
        ${target_name}.hpp
        ${target_name}.cpp
)
set_compile_options(${target_name})
target_include_directories(
        ${target_name}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)