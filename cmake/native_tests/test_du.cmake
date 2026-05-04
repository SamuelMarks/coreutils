file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/du_test_dir")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/du_test_dir/file.txt" "12345")
execute_process(
    COMMAND "${BIN_DIR}/du" "-s" "${CMAKE_CURRENT_BINARY_DIR}/du_test_dir"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "du returned non-zero exit code: ${res}")
endif()
if(out STREQUAL "")
    message(FATAL_ERROR "du output empty")
endif()
