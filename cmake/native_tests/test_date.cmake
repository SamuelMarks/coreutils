execute_process(
    COMMAND "${BIN_DIR}/date" "+%Y-%m"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "date returned non-zero exit code: ${res}")
endif()
string(REPLACE "\n" "" out "${out}")
string(REPLACE "\r" "" out "${out}")
if(NOT out MATCHES "^[0-9][0-9][0-9][0-9]-[0-9][0-9]$")
    message(FATAL_ERROR "date output incorrect: ${out}")
endif()
