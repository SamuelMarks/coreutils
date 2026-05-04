execute_process(
    COMMAND "${BIN_DIR}/timeout" "0.1" "${BIN_DIR}/sleep" "2"
    RESULT_VARIABLE res
)
# timeout exits with 124 on timeout
if(NOT res EQUAL 124)
    message(FATAL_ERROR "timeout returned unexpected exit code: ${res}")
endif()

execute_process(
    COMMAND "${BIN_DIR}/timeout" "2" "${BIN_DIR}/sleep" "0.1"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "timeout returned unexpected exit code: ${res}")
endif()
