cmake_minimum_required(VERSION 3.28)
project(TestCppProject VERSION 1.0.0 LANGUAGES CXX)

# Don't use C++ 20 modules
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

## C++ language configuration boilerplate, hide symbols by default
if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

include(compiler-warnings.cmake)
include(compiler-options.cmake)
add_library(warnings INTERFACE)
add_library(options INTERFACE)
setup_compiler_warnings(warnings)
setup_compiler_options(options)
include(../cmake/sanitizers.cmake)
setup_sanitizers(options)

# Dummy run to make sure reflection.hpp exists when compiling the test-cpp-project
add_custom_target(
        generate_dummy_reflection
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/include
        COMMAND $<TARGET_FILE:obsidian>
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/include/types.hpp
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/include
        compile-options=-DDONT_CRASH,-I${CMAKE_CURRENT_SOURCE_DIR}/include,-I${CMAKE_SOURCE_DIR}/include
        DEPENDS obsidian
)

add_executable(test-cpp-project src/main-test.cpp src/secondary-test.cpp include/types.hpp third-party/catch2/src/catch_amalgamated.cpp)
add_dependencies(test-cpp-project warnings options generate_dummy_reflection)
target_compile_features(test-cpp-project PRIVATE cxx_std_20)
target_compile_definitions(test-cpp-project PRIVATE CATCH_AMALGAMATED_CUSTOM_MAIN DONT_CRASH)
target_include_directories(test-cpp-project PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}/include)
target_include_directories(test-cpp-project PRIVATE third-party/catch2/include)
target_include_directories(test-cpp-project PRIVATE third-party/catch2/include/catch2)

function(get_include_directories OUT_GENERATOR TARGET)
    set(${OUT_GENERATOR} $<JOIN:$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>,,> PARENT_SCOPE)
endfunction()

function(get_definitions OUT_GENERATOR TARGET)
    set(${OUT_GENERATOR} $<JOIN:$<LIST:TRANSFORM,$<TARGET_PROPERTY:${TARGET},COMPILE_DEFINITIONS>,PREPEND,-D>,,> PARENT_SCOPE)
endfunction()

function(add_obsidian_test)
    cmake_parse_arguments(ARG "" "NAME;OUTPUT_DIR;EXPECTED_EXIT_CODE" "OBSIDIAN_ARGS" ${ARGN})

    # Join the list into a single space-separated string.
    list(JOIN ARG_OBSIDIAN_ARGS " " OBSIDIAN_ARGS_STR)

    set(EXTRA_ARGS "")
    if (DEFINED ARG_EXPECTED_EXIT_CODE)
        set(EXTRA_ARGS -DEXPECTED_EXIT_CODE=${ARG_EXPECTED_EXIT_CODE})
    else ()
        set(EXTRA_ARGS -DTEST_EXE=$<TARGET_FILE:test-cpp-project>)
    endif ()

    add_test(NAME ${ARG_NAME}
        COMMAND ${CMAKE_COMMAND}
            -DOBSIDIAN_EXE=$<TARGET_FILE:obsidian>
            -DOUTPUT_DIR=${ARG_OUTPUT_DIR}
            "-DOBSIDIAN_ARGS=${OBSIDIAN_ARGS_STR}"
            ${EXTRA_ARGS}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/run-obsidian-test.cmake
    )
endfunction()

get_include_directories(INCLUDE_DIRECTORIES test-cpp-project)
get_definitions(DEFINITIONS test-cpp-project)

add_obsidian_test(
    NAME cpp_test
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include
    OBSIDIAN_ARGS
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/include/types.hpp
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/include
        std=c++20
        compile-options=${DEFINITIONS},-Wall
        inc-dirs=${INCLUDE_DIRECTORIES}
)

add_obsidian_test(
    NAME cpp_test_dir
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include-dir
    OBSIDIAN_ARGS
        input-dir=${CMAKE_CURRENT_SOURCE_DIR}/include
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/include-dir
        compile-options=${DEFINITIONS},-Wall
        inc-dirs=${INCLUDE_DIRECTORIES}
)

add_obsidian_test(
    NAME cpp_test_compile_error
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include-error
    OBSIDIAN_ARGS
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/include-error/error-types.hpp
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/include-error
        compile-options=-Wall
        inc-dirs=${INCLUDE_DIRECTORIES}
    EXPECTED_EXIT_CODE 1
)

# ---- Category 1: Argument validation tests ----

# 1.1 Neither input-file nor input-dir provided
add_obsidian_test(
    NAME cpp_test_no_input
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    OBSIDIAN_ARGS
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    EXPECTED_EXIT_CODE 1
)

# 1.2 Both input-file and input-dir provided
add_obsidian_test(
    NAME cpp_test_both_inputs
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    OBSIDIAN_ARGS
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/include/types.hpp
        input-dir=${CMAKE_CURRENT_SOURCE_DIR}/include
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    EXPECTED_EXIT_CODE 1
)

# 1.3 Non-existent input file
add_obsidian_test(
    NAME cpp_test_bad_input_file
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    OBSIDIAN_ARGS
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/does-not-exist.hpp
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    EXPECTED_EXIT_CODE 1
)

# 1.4 Non-existent input directory
add_obsidian_test(
    NAME cpp_test_bad_input_dir
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    OBSIDIAN_ARGS
        input-dir=${CMAKE_CURRENT_SOURCE_DIR}/does-not-exist
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    EXPECTED_EXIT_CODE 1
)

# 1.5 Non-existent output directory
add_obsidian_test(
    NAME cpp_test_bad_output_dir
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    OBSIDIAN_ARGS
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/include/types.hpp
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/this-output-dir-does-not-exist
    EXPECTED_EXIT_CODE 1
)

# 1.6 Missing output directory argument
add_obsidian_test(
    NAME cpp_test_missing_output_dir
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    OBSIDIAN_ARGS
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/include/types.hpp
    EXPECTED_EXIT_CODE 1
)

# 1.7 Invalid C++ standard
add_obsidian_test(
    NAME cpp_test_bad_std
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/arg-validation
    OBSIDIAN_ARGS
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/include/types.hpp
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/arg-validation
        std=c++99
    EXPECTED_EXIT_CODE 1
)

# ---- Category 2: Compilation error tests ----

# 2.2 Syntax error in input file
add_obsidian_test(
    NAME cpp_test_syntax_error
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include-error
    OBSIDIAN_ARGS
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/include-error/syntax-error.hpp
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/include-error
        inc-dirs=${INCLUDE_DIRECTORIES}
    EXPECTED_EXIT_CODE 1
)

# 2.3 Missing include
add_obsidian_test(
    NAME cpp_test_missing_include
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include-error
    OBSIDIAN_ARGS
        input-file=${CMAKE_CURRENT_SOURCE_DIR}/include-error/missing-include.hpp
        output-dir=${CMAKE_CURRENT_BINARY_DIR}/include-error
        inc-dirs=${INCLUDE_DIRECTORIES}
    EXPECTED_EXIT_CODE 1
)
