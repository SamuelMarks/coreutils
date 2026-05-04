execute_process(
    COMMAND "${BIN_DIR}/env" "TEST_ENV_VAR=12345" "${BIN_DIR}/printenv" "TEST_ENV_VAR"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "env returned non-zero exit code: ${res}")
endif()
if(NOT out STREQUAL "12345")
    message(FATAL_ERROR "env TEST_ENV_VAR=12345 failed to set variable, got '${out}'")
endif()
