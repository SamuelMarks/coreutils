file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/touch_test.txt")
execute_process(
    COMMAND "${BIN_DIR}/touch" "${CMAKE_CURRENT_BINARY_DIR}/touch_test.txt"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "touch returned non-zero exit code: ${res}")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/touch_test.txt")
    message(FATAL_ERROR "touch failed to create file")
endif()
