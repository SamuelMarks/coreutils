execute_process(
    COMMAND "${BIN_DIR}/shuf" "-i" "1-3"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "shuf returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "[1-3]")
    message(FATAL_ERROR "shuf output incorrect: ${out}")
endif()
