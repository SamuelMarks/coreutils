file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/rmdir_test_dir")
execute_process(
    COMMAND "${BIN_DIR}/rmdir" "${CMAKE_CURRENT_BINARY_DIR}/rmdir_test_dir"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "rmdir returned non-zero exit code: ${res}")
endif()
if(IS_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/rmdir_test_dir")
    message(FATAL_ERROR "rmdir failed to remove directory")
endif()
