execute_process(
    COMMAND "${BIN_DIR}/lbracket" "1" "-eq" "1" "]"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "[ 1 -eq 1 ] returned non-zero exit code: ${res}")
endif()

execute_process(
    COMMAND "${BIN_DIR}/lbracket" "1" "-eq" "2" "]"
    RESULT_VARIABLE res
)
if(res EQUAL 0)
    message(FATAL_ERROR "[ 1 -eq 2 ] returned zero exit code")
endif()
