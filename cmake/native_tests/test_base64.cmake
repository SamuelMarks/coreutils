file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/b64_in.txt" "hello world")
execute_process(
    COMMAND "${BIN_DIR}/base64" "${CMAKE_CURRENT_BINARY_DIR}/b64_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "base64 returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "aGVsbG8gd29ybGQ=")
    message(FATAL_ERROR "base64 output incorrect: ${out}")
endif()

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/b64_decode_in.txt" "aGVsbG8gd29ybGQ=")
execute_process(
    COMMAND "${BIN_DIR}/base64" "-d" "${CMAKE_CURRENT_BINARY_DIR}/b64_decode_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "base64 -d returned non-zero exit code: ${res}")
endif()
if(NOT out STREQUAL "hello world")
    message(FATAL_ERROR "base64 -d output incorrect: ${out}")
endif()
