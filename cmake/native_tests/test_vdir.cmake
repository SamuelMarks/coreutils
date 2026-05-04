file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/vdir_test_dir")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/vdir_test_dir/file1.txt" "1")
execute_process(
    COMMAND "${BIN_DIR}/vdir" "${CMAKE_CURRENT_BINARY_DIR}/vdir_test_dir"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "vdir returned non-zero exit code: ${res}")
endif()
if(NOT out MATCHES "file1\\.txt")
    message(FATAL_ERROR "vdir output incorrect: ${out}")
endif()
