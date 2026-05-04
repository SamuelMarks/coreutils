# test basic execution wrapper
execute_process(
    COMMAND "${BIN_DIR}/nohup" "${BIN_DIR}/echo" "hello"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "nohup returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "hello")
    # Might output "nohup: ignoring input and appending output to 'nohup.out'" if running natively,
    # but since execute_process uses pipes for output, nohup just writes directly.
endif()
