execute_process(
    COMMAND "${BIN_DIR}/nice" "-n" "0" "${BIN_DIR}/echo" "hello"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "nice returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "hello")
    message(FATAL_ERROR "nice output incorrect: ${out}")
endif()
