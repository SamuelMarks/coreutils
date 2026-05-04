file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/paste_1.txt" "1\n2\n")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/paste_2.txt" "a\nb\n")
execute_process(
    COMMAND "${BIN_DIR}/paste" "${CMAKE_CURRENT_BINARY_DIR}/paste_1.txt" "${CMAKE_CURRENT_BINARY_DIR}/paste_2.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "paste returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out MATCHES "1\ta\n2\tb")
    message(FATAL_ERROR "paste output incorrect: ${out}")
endif()
