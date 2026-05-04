execute_process(
    COMMAND "${BIN_DIR}/sync"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "sync returned non-zero exit code: ${res}")
endif()
