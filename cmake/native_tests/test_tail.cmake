file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tail_test.txt" "line1\nline2\nline3\nline4\n")
execute_process(
    COMMAND "${BIN_DIR}/tail" "-n" "2" "${CMAKE_CURRENT_BINARY_DIR}/tail_test.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "tail returned non-zero exit code: ${res}")
endif()
# Windows CRLF handling
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "line3\nline4")
    message(FATAL_ERROR "tail -n 2 returned '${out}'")
endif()
