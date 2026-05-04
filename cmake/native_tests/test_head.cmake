file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/head_test.txt" "line1\nline2\nline3\nline4\n")
execute_process(
    COMMAND "${BIN_DIR}/head" "-n" "2" "${CMAKE_CURRENT_BINARY_DIR}/head_test.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "head returned non-zero exit code: ${res}")
endif()
# Windows CRLF handling
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "line1\nline2")
    message(FATAL_ERROR "head -n 2 returned '${out}'")
endif()
