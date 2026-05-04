file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/dd_in.txt" "hello world\n")
execute_process(
    COMMAND "${BIN_DIR}/dd" "if=${CMAKE_CURRENT_BINARY_DIR}/dd_in.txt" "of=${CMAKE_CURRENT_BINARY_DIR}/dd_out.txt" "status=none"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "dd returned non-zero exit code: ${res}")
endif()
file(READ "${CMAKE_CURRENT_BINARY_DIR}/dd_out.txt" out)
if(NOT out STREQUAL "hello world\n")
    message(FATAL_ERROR "dd output incorrect: ${out}")
endif()
