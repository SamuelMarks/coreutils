execute_process(
    COMMAND "${BIN_DIR}/users"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "users returned non-zero exit code: ${res}")
endif()
