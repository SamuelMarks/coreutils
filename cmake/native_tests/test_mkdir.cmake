file(REMOVE_RECURSE "${CMAKE_CURRENT_BINARY_DIR}/mkdir_test_dir")
execute_process(
    COMMAND "${BIN_DIR}/mkdir" "-p" "${CMAKE_CURRENT_BINARY_DIR}/mkdir_test_dir/a/b"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "mkdir returned non-zero exit code: ${res}")
endif()
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/mkdir_test_dir/a/b")
    message(FATAL_ERROR "mkdir did not create directory")
endif()
