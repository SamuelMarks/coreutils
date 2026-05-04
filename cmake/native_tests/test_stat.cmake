file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/stat_in.txt" "hello")
execute_process(
    COMMAND "${BIN_DIR}/stat" "-c" "%s" "${CMAKE_CURRENT_BINARY_DIR}/stat_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "stat returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "5")
    message(FATAL_ERROR "stat output incorrect: ${out}")
endif()
