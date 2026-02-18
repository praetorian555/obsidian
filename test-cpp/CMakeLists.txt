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

add_custom_target(
        generate_reflection
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/include
        COMMAND $<TARGET_FILE:obsidian>
            input-file=${CMAKE_CURRENT_SOURCE_DIR}/include/types.hpp
            output-dir=${CMAKE_CURRENT_BINARY_DIR}/include
            compile-options=-DDONT_CRASH,-I${CMAKE_CURRENT_SOURCE_DIR}/include,-I${CMAKE_SOURCE_DIR}/include
        DEPENDS obsidian
        COMMENT "Generating reflection headers..."
        VERBATIM
)

add_executable(test-cpp-project src/main.cpp src/secondary.cpp include/types.hpp third-party/catch2/src/catch_amalgamated.cpp)
add_dependencies(test-cpp-project warnings options generate_reflection)
target_compile_features(test-cpp-project PRIVATE cxx_std_20)
target_compile_definitions(test-cpp-project PRIVATE CATCH_AMALGAMATED_CUSTOM_MAIN DONT_CRASH)
target_include_directories(test-cpp-project PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}/include)
target_include_directories(test-cpp-project PRIVATE third-party/catch2/include)
target_include_directories(test-cpp-project PRIVATE third-party/catch2/include/catch2)

add_custom_target(
        generate_reflection_dir
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/include-dir
        COMMAND $<TARGET_FILE:obsidian>
            input-dir=${CMAKE_CURRENT_SOURCE_DIR}/include
            output-dir=${CMAKE_CURRENT_BINARY_DIR}/include-dir
            compile-options=-DDONT_CRASH,-I${CMAKE_CURRENT_SOURCE_DIR}/include,-I${CMAKE_SOURCE_DIR}/include
        DEPENDS obsidian
        COMMENT "Generating reflection headers using input-dir..."
        VERBATIM
)

add_executable(test-cpp-project-dir src/main.cpp src/secondary.cpp include/types.hpp third-party/catch2/src/catch_amalgamated.cpp)
add_dependencies(test-cpp-project-dir warnings options generate_reflection_dir)
target_compile_features(test-cpp-project-dir PRIVATE cxx_std_20)
target_compile_definitions(test-cpp-project-dir PRIVATE CATCH_AMALGAMATED_CUSTOM_MAIN DONT_CRASH)
target_include_directories(test-cpp-project-dir PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/include-dir ${CMAKE_SOURCE_DIR}/include)
target_include_directories(test-cpp-project-dir PRIVATE third-party/catch2/include)
target_include_directories(test-cpp-project-dir PRIVATE third-party/catch2/include/catch2)

enable_testing()
add_test(NAME cpp_test COMMAND test-cpp-project)
add_test(NAME cpp_test_dir COMMAND test-cpp-project-dir)
