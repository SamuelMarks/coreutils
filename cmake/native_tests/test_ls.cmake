file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/ls_test_dir")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ls_test_dir/file1.txt" "1")
execute_process(
    COMMAND "${BIN_DIR}/ls" "-1" "${CMAKE_CURRENT_BINARY_DIR}/ls_test_dir"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "ls returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "file1.txt")
    message(FATAL_ERROR "ls output incorrect: ${out}")
endif()
