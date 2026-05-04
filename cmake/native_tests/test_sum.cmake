file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/sum_in.txt" "hello")
execute_process(
    COMMAND "${BIN_DIR}/sum" "${CMAKE_CURRENT_BINARY_DIR}/sum_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "sum returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "^[0-9]+[ \t]+[0-9]+")
    message(FATAL_ERROR "sum output incorrect: ${out}")
endif()
