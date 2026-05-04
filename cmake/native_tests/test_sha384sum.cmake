file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/sha384_in.txt" "hello world\n")
execute_process(
    COMMAND "${BIN_DIR}/sha384sum" "${CMAKE_CURRENT_BINARY_DIR}/sha384_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "sha384sum returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "^[a-f0-9]+ [ \\*]?.*sha384_in\\.txt$")
    message(FATAL_ERROR "sha384sum output incorrect: ${out}")
endif()
