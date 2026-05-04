file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ptx_in.txt" "hello world\n")
execute_process(
    COMMAND "${BIN_DIR}/ptx" "${CMAKE_CURRENT_BINARY_DIR}/ptx_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "ptx returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "hello")
    message(FATAL_ERROR "ptx output incorrect: ${out}")
endif()
