file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/unlink_in.txt" "test")
execute_process(
    COMMAND "${BIN_DIR}/unlink" "${CMAKE_CURRENT_BINARY_DIR}/unlink_in.txt"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "unlink returned non-zero exit code: ${res}")
endif()
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/unlink_in.txt")
    message(FATAL_ERROR "unlink failed to remove file")
endif()
