file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/nl_in.txt" "line1\nline2\n")
execute_process(
    COMMAND "${BIN_DIR}/nl" "${CMAKE_CURRENT_BINARY_DIR}/nl_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "nl returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out MATCHES "1\tline1\n *2\tline2")
    message(FATAL_ERROR "nl output incorrect: '${out}'")
endif()
