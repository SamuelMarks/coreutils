execute_process(
    COMMAND "${BIN_DIR}/dircolors" "-b"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "dircolors returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "LS_COLORS=.*export LS_COLORS")
    message(FATAL_ERROR "dircolors output incorrect: ${out}")
endif()
