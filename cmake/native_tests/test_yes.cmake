execute_process(
    COMMAND "${BIN_DIR}/yes" "hello"
    COMMAND "${BIN_DIR}/head" "-n" "2"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Note: yes will be terminated by SIGPIPE from head, so we don't strictly check the result variable for 0
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "hello\nhello")
    message(FATAL_ERROR "yes hello | head -n 2 returned '${out}'")
endif()
