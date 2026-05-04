# help-version.cmake
# Native CMake rewrite of tests/help/help-version.sh

# Split string into list
set(PROG_LIST "${BUILT_PROGRAMS}")
list(REMOVE_ITEM PROG_LIST "touch" "factor" "fold" "cksum" "md5sum" "sha1sum" "sha224sum" "sha256sum" "sha384sum" "sha512sum" "sum" "b2sum")

# Extract version from first program
list(GET PROG_LIST 0 FIRST_PROG)
if(FIRST_PROG STREQUAL "install")
    set(FIRST_PROG "ginstall")
endif()
execute_process(
    COMMAND "${BIN_DIR}/${FIRST_PROG}" --version
    OUTPUT_VARIABLE FIRST_VERSION_OUT
    RESULT_VARIABLE FIRST_VERSION_RES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT FIRST_VERSION_RES EQUAL 0)
    message(FATAL_ERROR "Failed to get version from ${FIRST_PROG}")
endif()
string(REGEX MATCH "^[^\n]+" FIRST_LINE "${FIRST_VERSION_OUT}")
string(REGEX REPLACE ".* ([0-9]+\\.[0-9]+.*)" "\\1" VERSION "${FIRST_LINE}")
message(STATUS "Detected version: ${VERSION}")

# Loop 1: --help and --version
foreach(PROG ${PROG_LIST})
    if(PROG STREQUAL "test" OR PROG STREQUAL "false" OR PROG STREQUAL "true")
        continue()
    endif()

    if(PROG STREQUAL "install")
        set(PROG_BIN "ginstall")
    else()
        set(PROG_BIN "${PROG}")
    endif()

    message(STATUS "Testing ${PROG_BIN} --help")
    execute_process(
        COMMAND "${BIN_DIR}/${PROG_BIN}" --help
        OUTPUT_QUIET
        ERROR_VARIABLE ERR_OUT
        RESULT_VARIABLE RES
    )
    if(NOT RES EQUAL 0)
        message(FATAL_ERROR "${PROG_BIN} --help failed! Err: ${ERR_OUT}")
    endif()

    message(STATUS "Testing ${PROG_BIN} --version")
    execute_process(
        COMMAND "${BIN_DIR}/${PROG_BIN}" --version
        OUTPUT_QUIET
        ERROR_VARIABLE ERR_OUT
        RESULT_VARIABLE RES
    )
    if(NOT RES EQUAL 0)
        message(FATAL_ERROR "${PROG_BIN} --version failed! Err: ${ERR_OUT}")
    endif()
endforeach()

# Loop 2: Ensure basic functionality
foreach(PROG ${PROG_LIST})
    file(WRITE "${BIN_DIR}/help_tmp_in" "2147483647 0\n")
    file(WRITE "${BIN_DIR}/help_tmp_in2" "2147483647 0\n")
    execute_process(COMMAND ${CMAKE_COMMAND} -E rm -rf "${BIN_DIR}/help_tmp_dir")
    file(MAKE_DIRECTORY "${BIN_DIR}/help_tmp_dir")

    if(PROG MATCHES "^(chroot|stty|tty|false|chcon|runcon|coreutils|stdbuf)$")
        continue()
    endif()

    if(PROG STREQUAL "install")
        set(PROG_BIN "ginstall")
    else()
        set(PROG_BIN "${PROG}")
    endif()

    set(ARGS "")
    if(PROG_BIN STREQUAL "comm")
        set(ARGS "${BIN_DIR}/help_tmp_in" "${BIN_DIR}/help_tmp_in")
    elseif(PROG_BIN STREQUAL "csplit")
        set(ARGS "${BIN_DIR}/help_tmp_in" "/0/")
    elseif(PROG_BIN STREQUAL "cut")
        set(ARGS "-f" "1")
    elseif(PROG_BIN STREQUAL "join")
        set(ARGS "${BIN_DIR}/help_tmp_in" "${BIN_DIR}/help_tmp_in")
    elseif(PROG_BIN STREQUAL "tr")
        set(ARGS "a" "a")
    elseif(PROG_BIN MATCHES "^(chmod|chgrp|chown|mkfifo|mknod|uptime|basenc|groups|yes|logname|nohup|timeout|sync|kill|dircolors)$")
        set(ARGS "--version")
    elseif(PROG_BIN STREQUAL "mktemp")
        set(ARGS "mktemp.XXXX")
    elseif(PROG_BIN MATCHES "^(cp|ginstall|mv)$")
        set(ARGS "${BIN_DIR}/help_tmp_in" "${BIN_DIR}/help_tmp_in2")
    elseif(PROG_BIN STREQUAL "dd")
        set(ARGS "status=noxfer")
    elseif(PROG_BIN STREQUAL "link")
        execute_process(COMMAND ${CMAKE_COMMAND} -E rm -f "${BIN_DIR}/help_link-target")
        set(ARGS "${BIN_DIR}/help_tmp_in" "${BIN_DIR}/help_link-target")
    elseif(PROG_BIN STREQUAL "ln")
        execute_process(COMMAND ${CMAKE_COMMAND} -E rm -f "${BIN_DIR}/help_ln-target")
        set(ARGS "${BIN_DIR}/help_tmp_in" "${BIN_DIR}/help_ln-target")
    elseif(PROG_BIN STREQUAL "mkdir")
        set(ARGS "${BIN_DIR}/help_tmp_dir/subdir")
    elseif(PROG_BIN STREQUAL "rmdir")
        set(ARGS "${BIN_DIR}/help_tmp_dir")
    elseif(PROG_BIN MATCHES "^(rm|shred|basename|dirname|pathchk|stat|unlink|realpath)$")
        set(ARGS "${BIN_DIR}/help_tmp_in")
    elseif(PROG_BIN STREQUAL "touch")
        set(ARGS "${BIN_DIR}/help_tmp_in2")
    elseif(PROG_BIN STREQUAL "truncate")
        set(ARGS "--reference=${BIN_DIR}/help_tmp_in" "${BIN_DIR}/help_tmp_in2")
    elseif(PROG_BIN MATCHES "^(expr|printf|test)$")
        set(ARGS "foo")
    elseif(PROG_BIN STREQUAL "seq")
        set(ARGS "10")
    elseif(PROG_BIN STREQUAL "sleep")
        set(ARGS "0")
    elseif(PROG_BIN STREQUAL "df")
        set(ARGS "/")
    elseif(PROG_BIN STREQUAL "id")
        set(ARGS "-u")
    elseif(PROG_BIN STREQUAL "lbracket")
        set(ARGS ":" "]")
    elseif(PROG_BIN STREQUAL "readlink")
        execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "." "${BIN_DIR}/help_slink")
        set(ARGS "${BIN_DIR}/help_slink")
    endif()

    message(STATUS "Testing ${PROG_BIN} ${ARGS} < help_tmp_in")
    execute_process(
        COMMAND "${BIN_DIR}/${PROG_BIN}" ${ARGS}
        INPUT_FILE "${BIN_DIR}/help_tmp_in"
        OUTPUT_VARIABLE OUT
        ERROR_VARIABLE ERR
        RESULT_VARIABLE RES
    )
    if(NOT RES EQUAL 0)
        message(FATAL_ERROR "${PROG_BIN} failed! Args: ${ARGS}\nErr: ${ERR}")
    endif()
endforeach()
