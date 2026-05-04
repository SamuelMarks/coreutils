file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tee_in.txt" "a\nb\nc\n")
execute_process(
    COMMAND "${BIN_DIR}/tee" "${CMAKE_CURRENT_BINARY_DIR}/tee_out.txt"
    INPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/tee_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "tee returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "a\nb\nc")
    message(FATAL_ERROR "tee output incorrect: ${out}")
endif()
file(READ "${CMAKE_CURRENT_BINARY_DIR}/tee_out.txt" file_out)
string(REPLACE "\r" "" file_out "${file_out}")
if(NOT file_out STREQUAL "a\nb\nc\n")
    message(FATAL_ERROR "tee file output incorrect: ${file_out}")
endif()
