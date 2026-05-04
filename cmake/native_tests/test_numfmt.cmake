execute_process(
    COMMAND "${BIN_DIR}/numfmt" "--to=si" "1000"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "numfmt returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "1.0k" AND NOT out STREQUAL "1.0K")
    message(FATAL_ERROR "numfmt output incorrect: ${out}")
endif()
