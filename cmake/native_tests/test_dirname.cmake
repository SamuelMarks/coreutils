execute_process(
    COMMAND "${BIN_DIR}/dirname" "/usr/bin/sort"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "dirname returned non-zero")
endif()
if(NOT out STREQUAL "/usr/bin")
    message(FATAL_ERROR "dirname /usr/bin/sort returned '${out}' instead of '/usr/bin'")
endif()

execute_process(
    COMMAND "${BIN_DIR}/dirname" "stdio.h"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "dirname returned non-zero")
endif()
if(NOT out STREQUAL ".")
    message(FATAL_ERROR "dirname stdio.h returned '${out}' instead of '.'")
endif()
