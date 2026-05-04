execute_process(
    COMMAND "${BIN_DIR}/logname"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# logname can fail if not in a login session, so just test it gracefully handles execution
if(res EQUAL 0 AND out STREQUAL "")
    message(FATAL_ERROR "logname returned 0 but no output")
endif()
