execute_process(
    COMMAND "${BIN_DIR}/expr" "1" "+" "2"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "expr returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "3")
    message(FATAL_ERROR "expr output incorrect: ${out}")
endif()
