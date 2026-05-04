file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/pr_in.txt" "a\nb\nc\n")
execute_process(
    COMMAND "${BIN_DIR}/pr" "-t" "-n" "${CMAKE_CURRENT_BINARY_DIR}/pr_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "pr returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "1\ta")
    message(FATAL_ERROR "pr output incorrect: ${out}")
endif()
