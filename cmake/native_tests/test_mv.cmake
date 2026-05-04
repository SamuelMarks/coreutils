file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/mv_in.txt" "test mv content\n")
execute_process(
    COMMAND "${BIN_DIR}/mv" "${CMAKE_CURRENT_BINARY_DIR}/mv_in.txt" "${CMAKE_CURRENT_BINARY_DIR}/mv_out.txt"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "mv returned non-zero exit code: ${res}")
endif()
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/mv_in.txt")
    message(FATAL_ERROR "mv failed to remove source file")
endif()
file(READ "${CMAKE_CURRENT_BINARY_DIR}/mv_out.txt" out)
if(NOT out STREQUAL "test mv content\n")
    message(FATAL_ERROR "mv output incorrect: ${out}")
endif()
