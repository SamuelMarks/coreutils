file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/fmt_in.txt" "1234567890 1234567890\n")
execute_process(
    COMMAND "${BIN_DIR}/fmt" "-w" "15" "${CMAKE_CURRENT_BINARY_DIR}/fmt_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "fmt returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "1234567890\n1234567890")
    message(FATAL_ERROR "fmt output incorrect: ${out}")
endif()
