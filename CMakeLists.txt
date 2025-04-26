cmake_minimum_required(VERSION 3.28)
project(TestCppProject VERSION 1.0.0 LANGUAGES CXX)

# Don't use C++ 20 modules
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

## C++ language configuration boilerplate, hide symbols by default
if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

include(test-cpp/compiler-warnings.cmake)
include(test-cpp/compiler-options.cmake)
add_library(warnings INTERFACE)
add_library(options INTERFACE)
setup_compiler_warnings(warnings)
setup_compiler_options(options)

set(SYSTEM_INCLUDE_DIRS_ARG ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

if (NOT SYSTEM_INCLUDE_DIRS_ARG)
    set(SYSTEM_INCLUDE_DIRS_ARG "")
else ()
    set (SYSTEM_INCLUDE_DIRS_ARG --include-system-folders=${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_custom_target(
        generate_reflection
        COMMAND ${CMAKE_COMMAND} -E echo "Building obsidian..."
        COMMAND dotnet restore
        COMMAND dotnet build obsidian.csproj --no-restore /p:Configuration=$<CONFIG> -o ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>
        COMMAND ${CMAKE_COMMAND} -E echo "Running obsidian..."
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>/obsidian
            --search-path=${CMAKE_CURRENT_SOURCE_DIR}/test-cpp/include
            --destination-path=${CMAKE_CURRENT_BINARY_DIR}/include
            --include-folders=${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_BINARY_DIR}/include
            ${SYSTEM_INCLUDE_DIRS_ARG}
            --defines=DONT_CRASH
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating reflection headers before building test cpp project..."
        VERBATIM
)

add_executable(test-cpp-project test-cpp/src/main.cpp test-cpp/src/secondary.cpp test-cpp/include/types.hpp  test-cpp/third-party/catch2/src/catch_amalgamated.cpp)
add_dependencies(test-cpp-project warnings options generate_reflection)
target_compile_features(test-cpp-project PRIVATE cxx_std_20)
target_compile_definitions(test-cpp-project PRIVATE CATCH_AMALGAMATED_CUSTOM_MAIN DONT_CRASH)
target_include_directories(test-cpp-project PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test-cpp/include ${CMAKE_CURRENT_BINARY_DIR}/include)
target_include_directories(test-cpp-project PRIVATE test-cpp/third-party/catch2/include)
target_include_directories(test-cpp-project PRIVATE test-cpp/third-party/catch2/include/catch2)

enable_testing()
add_test(NAME cpp_test COMMAND test-cpp-project)
