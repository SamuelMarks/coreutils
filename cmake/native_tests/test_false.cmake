execute_process(
    COMMAND "${BIN_DIR}/false"
    RESULT_VARIABLE res
)
if(res EQUAL 0)
    message(FATAL_ERROR "false returned zero exit code")
endif()

# Also ensure it fails with --help and --version (per coreutils false-status.sh)
execute_process(
    COMMAND "${BIN_DIR}/false" --version
    RESULT_VARIABLE res
)
if(res EQUAL 0)
    message(FATAL_ERROR "false --version returned zero exit code")
endif()

execute_process(
    COMMAND "${BIN_DIR}/false" --help
    RESULT_VARIABLE res
)
if(res EQUAL 0)
    message(FATAL_ERROR "false --help returned zero exit code")
endif()
