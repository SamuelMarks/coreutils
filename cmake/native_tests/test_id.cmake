execute_process(
    COMMAND "${BIN_DIR}/id" "-u"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "id returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "^[0-9]+$")
    message(FATAL_ERROR "id output incorrect: ${out}")
endif()
