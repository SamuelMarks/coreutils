file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/realpath_target.txt" "data")
execute_process(
    COMMAND "${BIN_DIR}/realpath" "${CMAKE_CURRENT_BINARY_DIR}/realpath_target.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "realpath returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "realpath_target\\.txt$")
    message(FATAL_ERROR "realpath output incorrect: ${out}")
endif()
