cmake_minimum_required(VERSION 3.28)
project(OBSIDIAN VERSION 0.1.0 LANGUAGES CXX)

option(OBS_LLVM_PATH "Root path where LLVM binaries and libraries are installed" "")
message(STATUS "OBS_LLVM_PATH: ${OBS_LLVM_PATH}")
option(OBS_HARDENING "Enable hardening options" OFF)
message(STATUS "OBS_HARDENING: ${OBS_HARDENING}")
option(OBS_BUILD_TESTS "Build test targets" OFF)
message(STATUS "OBS_BUILD_TESTS: ${OBS_BUILD_TESTS}")

if (NOT OBS_LLVM_PATH)
    message(FATAL_ERROR "LLVM root path not specified. Please specify the path like this: -DOBS_LLVM_PATH=<path-to-llvm>...")
endif ()

if (NOT EXISTS "${OBS_LLVM_PATH}")
    message(FATAL_ERROR "OBS_LLVM_PATH does not exist: ${OBS_LLVM_PATH}!")
endif ()

if (NOT EXISTS "${OBS_LLVM_PATH}/bin")
    message(FATAL_ERROR "LLVM binary path does not exist!")
endif()

if (NOT EXISTS "${OBS_LLVM_PATH}/lib")
    message(FATAL_ERROR "LLVM library path does not exist!")
endif()

if (NOT EXISTS "${OBS_LLVM_PATH}/include")
    message(FATAL_ERROR "LLVM include path does not exist!")
endif()

# Don't use C++ 20 modules
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

## C++ language configuration boilerplate, hide symbols by default
if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

# Configure desired compilation options and warnings
include(cmake/compiler-warnings.cmake)
include(cmake/compiler-options.cmake)
add_library(obs_warnings INTERFACE)
add_library(obs_options INTERFACE)
setup_compiler_warnings(obs_warnings)
setup_compiler_options(obs_options)

if (OBS_HARDENING)
    include(cmake/sanitizers.cmake)
    setup_sanitizers(obs_options)
endif ()

include(FetchContent)
SET(OPAL_BUILD_TESTS OFF)
SET(OPAL_HARDENING ${OBS_HARDENING})
set(OPAL_SHARED_LIBS OFF)
FetchContent_Declare(
        opal
        GIT_REPOSITORY https://github.com/praetorian555/opal
        GIT_TAG main
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(opal)

add_executable(obsidian
    obsidian/obsidian-main.cpp
    obsidian/types.hpp
    obsidian/templates.hpp
    obsidian/generator.hpp
    obsidian/generator.cpp
)
target_compile_features(obsidian PRIVATE cxx_std_20)
target_compile_definitions(obsidian PRIVATE
    OBS_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    OBS_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    OBS_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)
target_link_libraries(obsidian PRIVATE ${OBS_LLVM_PATH}/lib/libclang.lib opal)
target_include_directories(obsidian PRIVATE ${OBS_LLVM_PATH}/include)
target_include_directories(obsidian PUBLIC include)

# Copy file before building the executable
add_custom_command(TARGET obsidian PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${OBS_LLVM_PATH}/bin/libclang.dll
        ${CMAKE_BINARY_DIR}/libclang.dll
        COMMENT "Copying libclang.dll to output directory"
)

if (OBS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test-cpp)
endif ()

# Install
install(TARGETS obsidian RUNTIME DESTINATION .)
install(FILES ${OBS_LLVM_PATH}/bin/libclang.dll DESTINATION .)
install(FILES include/obs/obs.hpp DESTINATION include/obs)
install(FILES LICENSE DESTINATION .)
install(FILES LLVM-LICENSE DESTINATION .)