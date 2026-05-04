file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/sha1_in.txt" "hello world\n")
execute_process(
    COMMAND "${BIN_DIR}/sha1sum" "${CMAKE_CURRENT_BINARY_DIR}/sha1_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "sha1sum returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "^22596363b3de40b06f981fb85d82312e8c0ed511 [ \\*]?.*sha1_in\\.txt$")
    message(FATAL_ERROR "sha1sum output incorrect: ${out}")
endif()
