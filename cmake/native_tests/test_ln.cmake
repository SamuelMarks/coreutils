file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ln_in.txt" "data\n")
file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/ln_out.txt")
execute_process(
    COMMAND "${BIN_DIR}/ln" "-s" "${CMAKE_CURRENT_BINARY_DIR}/ln_in.txt" "${CMAKE_CURRENT_BINARY_DIR}/ln_out.txt"
    RESULT_VARIABLE res
)
# On Windows, symlink might require admin privilege.
if(res EQUAL 0)
    file(READ "${CMAKE_CURRENT_BINARY_DIR}/ln_out.txt" out)
    if(NOT out STREQUAL "data\n")
        message(FATAL_ERROR "ln output incorrect: ${out}")
    endif()
endif()
