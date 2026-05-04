file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/md5_in.txt" "hello world\n")
execute_process(
    COMMAND "${BIN_DIR}/md5sum" "${CMAKE_CURRENT_BINARY_DIR}/md5_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "md5sum returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "^6f5902ac237024bdd0c176cb93063dc4 [ \\*]?.*md5_in\\.txt$")
    message(FATAL_ERROR "md5sum output incorrect: ${out}")
endif()
