file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/join_1.txt" "1 a\n2 b\n")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/join_2.txt" "1 x\n2 y\n")
execute_process(
    COMMAND "${BIN_DIR}/join" "${CMAKE_CURRENT_BINARY_DIR}/join_1.txt" "${CMAKE_CURRENT_BINARY_DIR}/join_2.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "join returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "1 a x\n2 b y")
    message(FATAL_ERROR "join output incorrect: ${out}")
endif()
