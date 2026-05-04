file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/rm_test.txt" "data\n")
execute_process(
    COMMAND "${BIN_DIR}/rm" "${CMAKE_CURRENT_BINARY_DIR}/rm_test.txt"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "rm returned non-zero exit code: ${res}")
endif()
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/rm_test.txt")
    message(FATAL_ERROR "rm failed to remove file")
endif()
