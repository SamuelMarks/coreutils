file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/b2_in.txt" "hello world\n")
execute_process(
    COMMAND "${BIN_DIR}/b2sum" "${CMAKE_CURRENT_BINARY_DIR}/b2_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "b2sum returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "^[a-f0-9]+  .*b2_in\\.txt$")
    message(FATAL_ERROR "b2sum output incorrect: ${out}")
endif()
