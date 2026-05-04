execute_process(
    COMMAND "${BIN_DIR}/chgrp" "--version"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "chgrp --version returned non-zero exit code: ${res}")
endif()
