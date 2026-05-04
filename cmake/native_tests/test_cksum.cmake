file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cksum_in.txt" "hello world\n")
execute_process(
    COMMAND "${BIN_DIR}/cksum" "${CMAKE_CURRENT_BINARY_DIR}/cksum_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "cksum returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "^[0-9]+ [0-9]+ .*cksum_in\\.txt$")
    message(FATAL_ERROR "cksum output incorrect: ${out}")
endif()
