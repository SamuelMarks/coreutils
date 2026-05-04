execute_process(
    COMMAND "${BIN_DIR}/sleep" "0.1"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "sleep 0.1 returned non-zero exit code: ${res}")
endif()

execute_process(
    COMMAND "${BIN_DIR}/sleep" "invalid_value"
    ERROR_QUIET
    RESULT_VARIABLE res
)
if(res EQUAL 0)
    message(FATAL_ERROR "sleep invalid_value returned zero exit code")
endif()
