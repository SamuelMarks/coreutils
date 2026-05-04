file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/truncate_test.txt" "hello")
execute_process(
    COMMAND "${BIN_DIR}/truncate" "-s" "2" "${CMAKE_CURRENT_BINARY_DIR}/truncate_test.txt"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "truncate returned non-zero exit code: ${res}")
endif()
file(READ "${CMAKE_CURRENT_BINARY_DIR}/truncate_test.txt" out)
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "he")
    message(FATAL_ERROR "truncate output incorrect: ${out}")
endif()
