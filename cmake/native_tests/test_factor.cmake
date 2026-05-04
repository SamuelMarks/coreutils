execute_process(
    COMMAND "${BIN_DIR}/factor" "15"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "factor returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "15: 3 5")
    message(FATAL_ERROR "factor output incorrect: ${out}")
endif()
