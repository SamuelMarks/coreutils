execute_process(
    COMMAND "${BIN_DIR}/tty"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# tty exits with 1 if not a tty, which is expected when run via execute_process
if(NOT res EQUAL 1 AND NOT res EQUAL 0)
    message(FATAL_ERROR "tty returned unexpected exit code: ${res}")
endif()
if(NOT out MATCHES "not a tty" AND NOT out MATCHES "/dev/")
    message(FATAL_ERROR "tty output incorrect: ${out}")
endif()
