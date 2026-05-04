file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tsort_in.txt" "a b\nb c\n")
execute_process(
    COMMAND "${BIN_DIR}/tsort" "${CMAKE_CURRENT_BINARY_DIR}/tsort_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "tsort returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "a\nb\nc")
    message(FATAL_ERROR "tsort output incorrect: ${out}")
endif()
