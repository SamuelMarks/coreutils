file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/link_in.txt" "data\n")
file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/link_out.txt")
execute_process(
    COMMAND "${BIN_DIR}/link" "${CMAKE_CURRENT_BINARY_DIR}/link_in.txt" "${CMAKE_CURRENT_BINARY_DIR}/link_out.txt"
    RESULT_VARIABLE res
)
if(res EQUAL 0)
    file(READ "${CMAKE_CURRENT_BINARY_DIR}/link_out.txt" out)
    if(NOT out STREQUAL "data\n")
        message(FATAL_ERROR "link output incorrect: ${out}")
    endif()
endif()
