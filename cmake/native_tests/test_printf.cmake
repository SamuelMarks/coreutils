execute_process(
    COMMAND "${BIN_DIR}/printf" "%s %d" "hello" "42"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "printf returned non-zero exit code: ${res}")
endif()
if(NOT out STREQUAL "hello 42")
    message(FATAL_ERROR "printf output incorrect: ${out}")
endif()
