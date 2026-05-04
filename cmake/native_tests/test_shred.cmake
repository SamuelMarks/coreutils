file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/shred_test.txt" "sensitive data\n")
execute_process(
    COMMAND "${BIN_DIR}/shred" "-u" "${CMAKE_CURRENT_BINARY_DIR}/shred_test.txt"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "shred returned non-zero exit code: ${res}")
endif()
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/shred_test.txt")
    message(FATAL_ERROR "shred failed to remove file")
endif()
