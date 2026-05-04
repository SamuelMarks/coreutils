execute_process(
    COMMAND "${BIN_DIR}/echo" "hello" "world"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "echo returned non-zero")
endif()
if(NOT out MATCHES "^hello world$")
    message(FATAL_ERROR "echo hello world returned '${out}'")
endif()

execute_process(
    COMMAND "${BIN_DIR}/echo" "-n" "hello"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "echo returned non-zero")
endif()
if(NOT out STREQUAL "hello")
    message(FATAL_ERROR "echo -n hello returned '${out}'")
endif()
