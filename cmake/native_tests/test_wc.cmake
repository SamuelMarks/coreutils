file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/wc_in.txt" "a b c\n")
execute_process(
    COMMAND "${BIN_DIR}/wc" "-w" "${CMAKE_CURRENT_BINARY_DIR}/wc_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "wc returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "3 .*wc_in\\.txt$")
    message(FATAL_ERROR "wc output incorrect: ${out}")
endif()
