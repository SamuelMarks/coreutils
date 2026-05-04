set(ENV{TEST_VAR_123} "test_value_456")
execute_process(
    COMMAND "${BIN_DIR}/printenv" "TEST_VAR_123"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "printenv returned non-zero exit code: ${res}")
endif()
if(NOT out STREQUAL "test_value_456")
    message(FATAL_ERROR "printenv TEST_VAR_123 returned '${out}' instead of 'test_value_456'")
endif()

execute_process(
    COMMAND "${BIN_DIR}/printenv" "NON_EXISTENT_VAR_ABCDEF"
    OUTPUT_QUIET
    ERROR_QUIET
    RESULT_VARIABLE res
)
if(res EQUAL 0)
    message(FATAL_ERROR "printenv on non-existent var returned zero exit code")
endif()
