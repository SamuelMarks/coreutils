file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/csplit_in.txt" "a\nb\n---\nc\nd\n")
execute_process(
    COMMAND "${BIN_DIR}/csplit" "-q" "${CMAKE_CURRENT_BINARY_DIR}/csplit_in.txt" "/---/"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "csplit returned non-zero exit code: ${res}")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/xx00" OR NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/xx01")
    message(FATAL_ERROR "csplit failed to create output files")
endif()
