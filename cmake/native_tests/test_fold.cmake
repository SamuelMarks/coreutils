file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/fold_in.txt" "1234567890\n")
execute_process(
    COMMAND "${BIN_DIR}/fold" "-w" "5" "${CMAKE_CURRENT_BINARY_DIR}/fold_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "fold returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "12345\n67890")
    message(FATAL_ERROR "fold output incorrect: ${out}")
endif()
