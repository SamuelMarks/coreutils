file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/sha256_in.txt" "hello world\n")
execute_process(
    COMMAND "${BIN_DIR}/sha256sum" "${CMAKE_CURRENT_BINARY_DIR}/sha256_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "sha256sum returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "^[a-f0-9]+ [ \\*]?.*sha256_in\\.txt$")
    message(FATAL_ERROR "sha256sum output incorrect: ${out}")
endif()
