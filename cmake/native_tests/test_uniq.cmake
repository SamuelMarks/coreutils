file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/uniq_in.txt" "a\na\nb\nb\n")
execute_process(
    COMMAND "${BIN_DIR}/uniq" "${CMAKE_CURRENT_BINARY_DIR}/uniq_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "uniq returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "a\nb")
    message(FATAL_ERROR "uniq output incorrect: ${out}")
endif()
