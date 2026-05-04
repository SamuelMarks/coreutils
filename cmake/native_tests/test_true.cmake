execute_process(
    COMMAND "${BIN_DIR}/true"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "true returned non-zero exit code: ${res}")
endif()
