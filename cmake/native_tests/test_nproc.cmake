execute_process(
    COMMAND "${BIN_DIR}/nproc"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "nproc returned non-zero exit code: ${res}")
endif()
if(out MATCHES "^[0-9]+$")
    # valid
else()
    message(FATAL_ERROR "nproc output '${out}' is not a valid number")
endif()
