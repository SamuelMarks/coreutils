file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/od_in.txt" "abcd")
execute_process(
    COMMAND "${BIN_DIR}/od" "-c" "${CMAKE_CURRENT_BINARY_DIR}/od_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "od returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "a   b   c   d")
    message(FATAL_ERROR "od output incorrect: ${out}")
endif()
