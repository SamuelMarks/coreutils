file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/readlink_target.txt" "data")
# CMake create_symlink works on platforms that support it
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink "readlink_target.txt" "${CMAKE_CURRENT_BINARY_DIR}/readlink_symlink"
)
execute_process(
    COMMAND "${BIN_DIR}/readlink" "${CMAKE_CURRENT_BINARY_DIR}/readlink_symlink"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(res EQUAL 0) # Could fail if symlink not supported
    string(REPLACE "\r" "" out "${out}")
    if(NOT out STREQUAL "readlink_target.txt")
        message(FATAL_ERROR "readlink output incorrect: ${out}")
    endif()
endif()
