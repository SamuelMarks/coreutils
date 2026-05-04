file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/sort_in.txt" "b\na\nc\n")
execute_process(
    COMMAND "${BIN_DIR}/sort" "${CMAKE_CURRENT_BINARY_DIR}/sort_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "sort returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "a\nb\nc")
    message(FATAL_ERROR "sort output incorrect: ${out}")
endif()
