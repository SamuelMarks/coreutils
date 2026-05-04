# test basic basename functionality
execute_process(
    COMMAND "${BIN_DIR}/basename" "/usr/bin/sort"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "basename returned non-zero")
endif()
if(NOT out STREQUAL "sort")
    message(FATAL_ERROR "basename /usr/bin/sort returned '${out}' instead of 'sort'")
endif()

execute_process(
    COMMAND "${BIN_DIR}/basename" "stdio.h" ".h"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "basename returned non-zero")
endif()
if(NOT out STREQUAL "stdio")
    message(FATAL_ERROR "basename stdio.h .h returned '${out}' instead of 'stdio'")
endif()
