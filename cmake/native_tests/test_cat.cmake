file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cat_in1.txt" "hello ")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cat_in2.txt" "world")
execute_process(
    COMMAND "${BIN_DIR}/cat" "${CMAKE_CURRENT_BINARY_DIR}/cat_in1.txt" "${CMAKE_CURRENT_BINARY_DIR}/cat_in2.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "cat returned non-zero exit code: ${res}")
endif()
if(NOT out STREQUAL "hello world")
    message(FATAL_ERROR "cat output incorrect: ${out}")
endif()
