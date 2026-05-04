file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tr_in.txt" "hello")
execute_process(
    COMMAND "${BIN_DIR}/tr" "a-z" "A-Z"
    INPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/tr_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "tr returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "HELLO")
    message(FATAL_ERROR "tr output incorrect: ${out}")
endif()
