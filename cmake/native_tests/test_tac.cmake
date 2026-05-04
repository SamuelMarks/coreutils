file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tac_in.txt" "a\nb\nc\n")
execute_process(
    COMMAND "${BIN_DIR}/tac" "${CMAKE_CURRENT_BINARY_DIR}/tac_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "tac returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "c\nb\na")
    message(FATAL_ERROR "tac output incorrect: ${out}")
endif()
