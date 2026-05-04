file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/b32_in.txt" "hello world")
execute_process(
    COMMAND "${BIN_DIR}/base32" "${CMAKE_CURRENT_BINARY_DIR}/b32_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "base32 returned non-zero exit code: ${res}")
endif()
string(REPLACE "\r" "" out "${out}")
if(NOT out STREQUAL "NBSWY3DPEB3W64TMMQ======")
    message(FATAL_ERROR "base32 output incorrect: ${out}")
endif()

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/b32_decode_in.txt" "NBSWY3DPEB3W64TMMQ======")
execute_process(
    COMMAND "${BIN_DIR}/base32" "-d" "${CMAKE_CURRENT_BINARY_DIR}/b32_decode_in.txt"
    OUTPUT_VARIABLE out
    RESULT_VARIABLE res
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "base32 -d returned non-zero exit code: ${res}")
endif()
if(NOT out STREQUAL "hello world")
    message(FATAL_ERROR "base32 -d output incorrect: ${out}")
endif()
