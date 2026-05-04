file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cut_in.txt" "a,b,c\n1,2,3\n")
execute_process(
    COMMAND "${BIN_DIR}/cut" "-d," "-f2" "${CMAKE_CURRENT_BINARY_DIR}/cut_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "cut returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "b\n2")
    message(FATAL_ERROR "cut output incorrect: ${out}")
endif()
