file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/install_in.txt" "hello")
file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/install_out.txt")
execute_process(
    COMMAND "${BIN_DIR}/ginstall" "${CMAKE_CURRENT_BINARY_DIR}/install_in.txt" "${CMAKE_CURRENT_BINARY_DIR}/install_out.txt"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "ginstall returned non-zero exit code: ${res}")
endif()
file(READ "${CMAKE_CURRENT_BINARY_DIR}/install_out.txt" out)
if(NOT out STREQUAL "hello")
    message(FATAL_ERROR "ginstall output incorrect: ${out}")
endif()
