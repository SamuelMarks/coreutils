file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cp_in.txt" "test cp content\n")
file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/cp_out.txt")
execute_process(
    COMMAND "${BIN_DIR}/cp" "${CMAKE_CURRENT_BINARY_DIR}/cp_in.txt" "${CMAKE_CURRENT_BINARY_DIR}/cp_out.txt"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "cp returned non-zero exit code: ${res}")
endif()
file(READ "${CMAKE_CURRENT_BINARY_DIR}/cp_out.txt" out)
if(NOT out STREQUAL "test cp content\n")
    message(FATAL_ERROR "cp output incorrect: ${out}")
endif()
