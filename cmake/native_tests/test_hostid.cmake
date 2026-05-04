execute_process(
    COMMAND "${BIN_DIR}/hostid"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "hostid returned non-zero exit code: ${res}")
endif()
if(out STREQUAL "")
    message(FATAL_ERROR "hostid returned empty string")
endif()
