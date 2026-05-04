# Note: stdbuf on Windows via native port might not be fully functional,
# but we can test if it executes properly.
execute_process(
    COMMAND "${BIN_DIR}/stdbuf" "-o0" "${BIN_DIR}/echo" "hello"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "stdbuf returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "hello")
    message(FATAL_ERROR "stdbuf output incorrect: ${out}")
endif()
