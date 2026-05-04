execute_process(
    COMMAND "${BIN_DIR}/stty"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
# stty often exits with 1 if not attached to a terminal
if(NOT res EQUAL 0 AND NOT res EQUAL 1)
    message(FATAL_ERROR "stty returned unexpected exit code: ${res}")
endif()
