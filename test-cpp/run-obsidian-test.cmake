# run-obsidian-test.cmake
# CMake script executed via cmake -P to run obsidian and optionally a test executable.
#
# Expected variables (passed via -D):
#   OBSIDIAN_EXE  - Path to the obsidian executable
#   OUTPUT_DIR    - Directory to create for obsidian output
#   OBSIDIAN_ARGS - Space-separated obsidian arguments (key=value format)
#   TEST_EXE           - (Optional) Path to a test executable to run after obsidian
#   EXPECTED_EXIT_CODE - (Optional) Expected obsidian exit code (default: 0)

if (NOT DEFINED OBSIDIAN_EXE)
    message(FATAL_ERROR "OBSIDIAN_EXE is not defined")
endif ()
if (NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "OUTPUT_DIR is not defined")
endif ()
if (NOT DEFINED OBSIDIAN_ARGS)
    message(FATAL_ERROR "OBSIDIAN_ARGS is not defined")
endif ()

# Create the output directory.
file(MAKE_DIRECTORY "${OUTPUT_DIR}")

# Split the space-separated args back into a list.
separate_arguments(ARG_LIST NATIVE_COMMAND "${OBSIDIAN_ARGS}")

# Run obsidian.
execute_process(
    COMMAND "${OBSIDIAN_EXE}" ${ARG_LIST}
    RESULT_VARIABLE OBSIDIAN_RESULT
)

if (DEFINED EXPECTED_EXIT_CODE)
    if (NOT OBSIDIAN_RESULT EQUAL ${EXPECTED_EXIT_CODE})
        message(FATAL_ERROR "obsidian exited with code ${OBSIDIAN_RESULT}, expected ${EXPECTED_EXIT_CODE}")
    endif ()
else ()
    if (NOT OBSIDIAN_RESULT EQUAL 0)
        message(FATAL_ERROR "obsidian failed with exit code ${OBSIDIAN_RESULT}")
    endif ()
endif ()

# Optionally run the test executable.
if (DEFINED TEST_EXE)
    execute_process(
        COMMAND "${TEST_EXE}"
        RESULT_VARIABLE TEST_RESULT
    )
    if (NOT TEST_RESULT EQUAL 0)
        message(FATAL_ERROR "Test executable failed with exit code ${TEST_RESULT}")
    endif ()
endif ()
