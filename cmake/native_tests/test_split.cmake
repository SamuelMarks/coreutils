file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/split_in.txt" "1\n2\n3\n4\n5\n")
execute_process(
    COMMAND "${BIN_DIR}/split" "-l" "2" "${CMAKE_CURRENT_BINARY_DIR}/split_in.txt" "split_out_"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "split returned non-zero exit code: ${res}")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/split_out_aa")
    message(FATAL_ERROR "split failed to create split_out_aa")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/split_out_ab")
    message(FATAL_ERROR "split failed to create split_out_ab")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/split_out_ac")
    message(FATAL_ERROR "split failed to create split_out_ac")
endif()
