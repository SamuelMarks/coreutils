execute_process(
    COMMAND "${BIN_DIR}/seq" "3"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "seq returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "1\n2\n3")
    message(FATAL_ERROR "seq output incorrect: ${out}")
endif()
