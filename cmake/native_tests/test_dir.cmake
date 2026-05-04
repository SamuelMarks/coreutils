file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dir_test_dir")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/dir_test_dir/file1.txt" "1")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/dir_test_dir/file2.txt" "2")
execute_process(
    COMMAND "${BIN_DIR}/dir" "-1" "${CMAKE_CURRENT_BINARY_DIR}/dir_test_dir"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "dir returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "file1.txt\nfile2.txt")
    message(FATAL_ERROR "dir output incorrect: ${out}")
endif()
