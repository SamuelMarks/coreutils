file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/unexpand_in.txt" "a       b\n")
execute_process(
    COMMAND "${BIN_DIR}/unexpand" "-a" "${CMAKE_CURRENT_BINARY_DIR}/unexpand_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "unexpand returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out MATCHES "a\tb")
    message(FATAL_ERROR "unexpand output incorrect: ${out}")
endif()
