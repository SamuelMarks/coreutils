file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/expand_in.txt" "a\tb\n")
execute_process(
    COMMAND "${BIN_DIR}/expand" "-t4" "${CMAKE_CURRENT_BINARY_DIR}/expand_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "expand returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "a   b")
    message(FATAL_ERROR "expand output incorrect: '${out}'")
endif()
