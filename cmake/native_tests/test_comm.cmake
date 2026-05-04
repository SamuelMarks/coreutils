file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/comm_1.txt" "a\nb\nc\n")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/comm_2.txt" "b\nc\nd\n")
execute_process(
    COMMAND "${BIN_DIR}/comm" "${CMAKE_CURRENT_BINARY_DIR}/comm_1.txt" "${CMAKE_CURRENT_BINARY_DIR}/comm_2.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "comm returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "a\n\t\tb\n\t\tc\n\td")
    message(FATAL_ERROR "comm output incorrect: '${out}'")
endif()
