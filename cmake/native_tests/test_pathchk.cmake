execute_process(
    COMMAND "${BIN_DIR}/pathchk" "valid_path/test.txt"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "pathchk valid returned non-zero exit code: ${res}")
endif()

execute_process(
    COMMAND "${BIN_DIR}/pathchk" ""
    ERROR_QUIET
    RESULT_VARIABLE res
)
if(res EQUAL 0)
    message(FATAL_ERROR "pathchk invalid returned zero exit code")
endif()
