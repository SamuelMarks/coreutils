file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/chmod_test.txt" "hello")
execute_process(
    COMMAND "${BIN_DIR}/chmod" "a+r" "${CMAKE_CURRENT_BINARY_DIR}/chmod_test.txt"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "chmod returned non-zero exit code: ${res}")
endif()
