execute_process(
    COMMAND "${BIN_DIR}/who"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "who returned non-zero exit code: ${res}")
endif()
