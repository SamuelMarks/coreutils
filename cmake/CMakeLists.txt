cmake_minimum_required(VERSION 3.15)
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()
project(coreutils C)

set(CMAKE_C_STANDARD 23)
add_compile_definitions(
    PKGLIBEXECDIR="${CMAKE_BINARY_DIR}/src"
    HAVE_CONFIG_H)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/src)

include_directories(${CMAKE_SOURCE_DIR}/../lib)
include_directories(${CMAKE_SOURCE_DIR}/../src)
include_directories(${CMAKE_SOURCE_DIR}/../gl/lib)
include_directories(${CMAKE_SOURCE_DIR}/../gnulib/lib)
if(DEFINED GNULIB_INCLUDE_DIR)
    include_directories(${GNULIB_INCLUDE_DIR})
else()
    include_directories(${CMAKE_BINARY_DIR}/_deps/coreutils_release-src/lib)
endif()
include_directories(${CMAKE_BINARY_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_BINARY_DIR}/src)

file(WRITE ${CMAKE_BINARY_DIR}/version.c "const char *Version = \"9.4\";\n")
file(WRITE ${CMAKE_BINARY_DIR}/version.h "extern const char *Version;\n")
add_compile_definitions(
    PKGLIBEXECDIR="${CMAKE_BINARY_DIR}/src"
    _GNU_SOURCE
    PROG_NAME="${PROJECT_NAME}"
    GNULIB_TEXT_DOMAIN="coreutils"
    POSIX_SPAWN_USEVFORK=0
    MANUAL_URL="https://www.gnu.org/software/coreutils/manual/coreutils.html"
)

enable_testing()


# Build common libraries

include(FetchContent)
FetchContent_Declare(
  coreutils_release
  URL https://ftp.gnu.org/gnu/coreutils/coreutils-9.4.tar.xz
)
FetchContent_MakeAvailable(coreutils_release)
include_directories(${coreutils_release_SOURCE_DIR}/lib)

# Patch posixtm.c for macOS AppleClang 15 and Linux Clang
file(READ ${coreutils_release_SOURCE_DIR}/lib/posixtm.c POSIXTM_C)
string(REPLACE "ckd_add (&t, t, leapsec)" "ckd_add (&t, t, (leapsec ? 1 : 0))" POSIXTM_C "${POSIXTM_C}")
file(WRITE ${coreutils_release_SOURCE_DIR}/lib/posixtm.c "${POSIXTM_C}")

if(APPLE)
    file(COPY ${CMAKE_SOURCE_DIR}/gnulib-macos/ DESTINATION ${CMAKE_BINARY_DIR}/lib)
    file(READ ${CMAKE_BINARY_DIR}/lib/configmake.h _CONFIGMAKE_CONTENT)
    string(REPLACE "/usr/local/libexec/coreutils" "${CMAKE_BINARY_DIR}/src" _CONFIGMAKE_CONTENT "${_CONFIGMAKE_CONTENT}")
    file(WRITE ${CMAKE_BINARY_DIR}/lib/configmake.h "${_CONFIGMAKE_CONTENT}")
else()
    file(COPY ${CMAKE_SOURCE_DIR}/gnulib-alpine/ DESTINATION ${CMAKE_BINARY_DIR}/lib)
endif()
file(REMOVE ${CMAKE_BINARY_DIR}/lib/xdectoint.h)
file(WRITE ${CMAKE_BINARY_DIR}/lib/gmp.h "#include \"mini-gmp.h\"\n")




file(APPEND ${CMAKE_BINARY_DIR}/lib/config.h "
#ifdef __linux__
# include <features.h>
#endif
#include <string.h>
#include <stdbool.h>
#define streq(a, b) (strcmp(a, b) == 0)\n#define STREQ(a, b) (strcmp(a, b) == 0)
#define memeq(a, b, n) (memcmp(a, b, n) == 0)
static inline bool str_endswith(const char *s, const char *sfx) {
    size_t l1 = strlen(s);
    size_t l2 = strlen(sfx);
    return l1 >= l2 && strcmp(s + l1 - l2, sfx) == 0;
}
")

if(NOT EXISTS "/etc/alpine-release")
    file(APPEND ${CMAKE_BINARY_DIR}/lib/config.h "
#undef HAVE___FSETERR
#undef HAVE___FREADAHEAD
#undef HAVE___FREADPTR
#undef HAVE___FREADPTRINC
#undef HAVE___FREADSEEK
#undef __GETOPT_PREFIX
#define __GETOPT_PREFIX
")
endif()

file(APPEND ${CMAKE_BINARY_DIR}/lib/config.h "
#ifndef S_IXUGO
# define S_IXUGO (S_IXUSR | S_IXGRP | S_IXOTH)
#endif
#ifndef S_IRWXUGO
# define S_IRWXUGO (S_IRWXU | S_IRWXG | S_IRWXO)
#endif
#ifndef S_TYPEISTMO
# define S_TYPEISTMO(p) 0
#endif
#ifndef _REGEX_NELTS
# define _REGEX_NELTS(n) n
#endif
#ifndef SIG2STR_MAX
# define SIG2STR_MAX (5 + 1 + 10 + 1 + 2)
#endif
extern int sig2str (int, char *);
extern int str2sig (char const *, int *);
#undef psame_inode
#include \"verror.h\"
#ifndef lgetfilecon_raw
# define lgetfilecon_raw lgetfilecon
#endif
#ifndef setfscreatecon_raw
# define setfscreatecon_raw setfscreatecon
#endif
")

include(${CMAKE_SOURCE_DIR}/gnulib_sources.cmake)
list(REMOVE_ITEM LIB_SOURCES "${coreutils_release_SOURCE_DIR}/lib/mini-gmp.c")

if(NOT EXISTS "/etc/alpine-release")
    list(REMOVE_ITEM LIB_SOURCES "${coreutils_release_SOURCE_DIR}/lib/getopt.c")
    list(REMOVE_ITEM LIB_SOURCES "${coreutils_release_SOURCE_DIR}/lib/getopt1.c")
    file(REMOVE ${CMAKE_BINARY_DIR}/lib/getopt.h)
    file(WRITE ${CMAKE_BINARY_DIR}/lib/getopt-ext.h "/* dummy to override gnulib/lib/getopt-ext.h */\n")

    list(APPEND LIB_SOURCES "${coreutils_release_SOURCE_DIR}/lib/fseterr.c")
    list(APPEND LIB_SOURCES "${coreutils_release_SOURCE_DIR}/lib/freadahead.c")
    list(APPEND LIB_SOURCES "${coreutils_release_SOURCE_DIR}/lib/freadptr.c")
    
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        file(APPEND ${CMAKE_BINARY_DIR}/lib/config.h "
#define STRERROR_R_CHAR_P 1
")
    endif()
endif()

set(GLLIB_SOURCES ${CMAKE_SOURCE_DIR}/../gl/lib/dtimespec-bound.c)
list(APPEND LIB_SOURCES ${GLLIB_SOURCES})

add_library(coreutils_lib STATIC ${LIB_SOURCES})
set_property(TARGET coreutils_lib PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(coreutils_lib PRIVATE ${CMAKE_BINARY_DIR}/lib ${CMAKE_SOURCE_DIR}/../gl/lib ${CMAKE_SOURCE_DIR}/../gnulib/lib ${coreutils_release_SOURCE_DIR}/lib)
find_package(Iconv REQUIRED)
find_library(INTL_LIBRARY intl)
find_library(GMP_LIBRARY gmp)
if(INTL_LIBRARY)
    target_link_libraries(coreutils_lib PRIVATE ${INTL_LIBRARY} Iconv::Iconv ${GMP_LIBRARY})
else()
    target_link_libraries(coreutils_lib PRIVATE Iconv::Iconv ${GMP_LIBRARY})
endif()
target_compile_definitions(coreutils_lib PRIVATE HAVE_CONFIG_H )

find_program(BISON_EXECUTABLE bison
    PATHS /opt/homebrew/opt/bison/bin /usr/local/opt/bison/bin
    NO_DEFAULT_PATH)
if(NOT BISON_EXECUTABLE)
    find_program(BISON_EXECUTABLE bison REQUIRED)
endif()
execute_process(COMMAND ${BISON_EXECUTABLE} -o ${CMAKE_BINARY_DIR}/parse-datetime.c ${CMAKE_SOURCE_DIR}/../gnulib/lib/parse-datetime.y)
add_library(parse_datetime_obj OBJECT ${CMAKE_BINARY_DIR}/parse-datetime.c)
set_property(TARGET parse_datetime_obj PROPERTY POSITION_INDEPENDENT_CODE ON)
set_target_properties(parse_datetime_obj PROPERTIES COMPILE_FLAGS "-w")
target_include_directories(parse_datetime_obj PRIVATE ${CMAKE_BINARY_DIR}/lib ${CMAKE_SOURCE_DIR}/../gnulib/lib ${coreutils_release_SOURCE_DIR}/lib)
target_compile_definitions(parse_datetime_obj PRIVATE HAVE_CONFIG_H)
add_library(version_obj OBJECT ${CMAKE_BINARY_DIR}/version.c)
set_property(TARGET version_obj PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(arch
    ../src/uname.c
    ../src/uname-arch.c
)
target_sources(arch PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(arch PRIVATE coreutils_lib)

add_executable(hostname
    ../src/hostname.c
)
target_sources(hostname PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(hostname PRIVATE coreutils_lib)

add_executable(kill
    ../src/kill.c
    ../src/operand2sig.c
)
target_sources(kill PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(kill PRIVATE coreutils_lib)

add_executable(uptime
    ../src/uptime.c
)
target_sources(uptime PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(uptime PRIVATE coreutils_lib)

add_executable(chroot
    ../src/chroot.c
)
target_sources(chroot PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(chroot PRIVATE coreutils_lib)

add_executable(df
    ../src/df.c
    ../src/find-mount-point.c
)
target_sources(df PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(df PRIVATE coreutils_lib)

add_executable(hostid
    ../src/hostid.c
)
target_sources(hostid PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(hostid PRIVATE coreutils_lib)

add_library(libstdbuf SHARED
    ../src/libstdbuf.c
)
target_sources(libstdbuf PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(libstdbuf PRIVATE coreutils_lib)

add_executable(nice
    ../src/nice.c
)
target_sources(nice PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(nice PRIVATE coreutils_lib)

add_executable(pinky
    ../src/pinky.c
)
target_sources(pinky PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(pinky PRIVATE coreutils_lib)

add_executable(stdbuf
    ../src/stdbuf.c
)
target_sources(stdbuf PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(stdbuf PRIVATE coreutils_lib)



add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/speedlist.h
    COMMAND ${CMAKE_C_COMPILER} -I${CMAKE_BINARY_DIR}/lib -I${CMAKE_SOURCE_DIR}/../lib -E -dM ${CMAKE_SOURCE_DIR}/../src/termios.c > ${CMAKE_BINARY_DIR}/speedlist.tmp
    COMMAND ${CMAKE_SOURCE_DIR}/../src/speedgen ${CMAKE_BINARY_DIR}/speedlist.h < ${CMAKE_BINARY_DIR}/speedlist.tmp
    DEPENDS ${CMAKE_SOURCE_DIR}/../src/speedgen ${CMAKE_SOURCE_DIR}/../src/termios.c
)
set_source_files_properties(../src/stty.c PROPERTIES OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/speedlist.h)

add_executable(stty
    ../src/stty.c
)
target_sources(stty PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(stty PRIVATE coreutils_lib)

add_executable(timeout
    ../src/timeout.c
    ../src/operand2sig.c
)
target_sources(timeout PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(timeout PRIVATE coreutils_lib)

add_executable(users
    ../src/users.c
)
target_sources(users PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(users PRIVATE coreutils_lib)

add_executable(who
    ../src/who.c
)
target_sources(who PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(who PRIVATE coreutils_lib)

add_executable(chcon
    ../src/chcon.c
)
target_sources(chcon PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(chcon PRIVATE coreutils_lib)

add_executable(runcon
    ../src/runcon.c
)
target_sources(runcon PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(runcon PRIVATE coreutils_lib)

add_executable(lbracket
    ../src/lbracket.c
)
target_sources(lbracket PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(lbracket PRIVATE coreutils_lib)

add_executable(b2sum
    ../src/cksum.c
    ../src/blake2/blake2b-ref.c
    ../src/blake2/b2sum.c
)
target_sources(b2sum PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(b2sum PRIVATE HASH_ALGO_BLAKE2=1)
target_compile_definitions(b2sum PRIVATE HAVE_CONFIG_H)
target_link_libraries(b2sum PRIVATE coreutils_lib)

add_executable(base64
    ../src/basenc.c
)
target_sources(base64 PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(base64 PRIVATE BASE_TYPE=64)
target_link_libraries(base64 PRIVATE coreutils_lib)

add_executable(base32
    ../src/basenc.c
)
target_sources(base32 PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(base32 PRIVATE BASE_TYPE=32)
target_link_libraries(base32 PRIVATE coreutils_lib)

add_executable(basenc
    ../src/basenc.c
)
target_sources(basenc PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(basenc PRIVATE BASE_TYPE=42)
target_link_libraries(basenc PRIVATE coreutils_lib)

add_executable(basename
    ../src/basename.c
)
target_sources(basename PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(basename PRIVATE coreutils_lib)

add_executable(cat
    ../src/cat.c
)
target_sources(cat PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(cat PRIVATE coreutils_lib)

add_executable(chgrp
    ../src/chown.c
    ../src/chown-core.c
    ../src/chown-chgrp.c
)
target_sources(chgrp PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(chgrp PRIVATE coreutils_lib)

add_executable(chmod
    ../src/chmod.c
)
target_sources(chmod PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(chmod PRIVATE coreutils_lib)

add_executable(chown
    ../src/chown.c
    ../src/chown-core.c
    ../src/chown-chown.c
)
target_sources(chown PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(chown PRIVATE coreutils_lib)

add_executable(cksum
    ../src/cksum.c
    ../src/blake2/blake2b-ref.c
    ../src/blake2/b2sum.c
    ../src/sum.c
    ../src/cksum_crc.c
    ${coreutils_release_SOURCE_DIR}/src/crctab.c
)
target_sources(cksum PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(cksum PRIVATE HASH_ALGO_CKSUM=1)
target_compile_definitions(cksum PRIVATE HAVE_CONFIG_H)
target_link_libraries(cksum PRIVATE coreutils_lib)

add_executable(comm
    ../src/comm.c
)
target_sources(comm PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(comm PRIVATE coreutils_lib)

add_executable(cp
    ../src/cp.c
    ../src/copy.c
    ../src/copy-file-data.c
    ../src/cp-hash.c
    ../src/force-link.c
    ../src/selinux.c
)
target_sources(cp PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(cp PRIVATE coreutils_lib)

add_executable(csplit
    ../src/csplit.c
)
target_sources(csplit PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(csplit PRIVATE coreutils_lib)

add_executable(cut
    ../src/cut.c
    ../src/set-fields.c
)
target_sources(cut PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(cut PRIVATE coreutils_lib)

add_executable(date
    ../src/date.c
    ../src/show-date.c
)
target_sources(date PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(date PRIVATE coreutils_lib)

add_executable(dd
    ../src/dd.c
)
target_sources(dd PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(dd PRIVATE coreutils_lib)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/src/dircolors.h
    COMMAND perl ${CMAKE_SOURCE_DIR}/../src/dcgen ${CMAKE_SOURCE_DIR}/../src/dircolors.hin > ${CMAKE_BINARY_DIR}/src/dircolors.h
    DEPENDS ${CMAKE_SOURCE_DIR}/../src/dcgen ${CMAKE_SOURCE_DIR}/../src/dircolors.hin
)
set_source_files_properties(../src/ls.c ../src/dircolors.c PROPERTIES OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/src/dircolors.h)

add_executable(dir
    ../src/ls.c
    ../src/ls-dir.c
)
target_sources(dir PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(dir PRIVATE coreutils_lib)

add_executable(dircolors
    ../src/dircolors.c
)
target_sources(dircolors PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(dircolors PRIVATE coreutils_lib)

add_executable(dirname
    ../src/dirname.c
)
target_sources(dirname PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(dirname PRIVATE coreutils_lib)

add_executable(du
    ../src/du.c
    ../src/show-date.c
)
target_sources(du PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(du PRIVATE coreutils_lib)

add_executable(echo
    ../src/echo.c
)
target_sources(echo PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(echo PRIVATE coreutils_lib)

add_executable(env
    ../src/env.c
    ../src/operand2sig.c
)
target_sources(env PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(env PRIVATE coreutils_lib)

add_executable(expand
    ../src/expand.c
    ../src/expand-common.c
)
target_sources(expand PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(expand PRIVATE coreutils_lib)

add_executable(expr
    ../src/expr.c
)
target_sources(expr PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(expr PRIVATE coreutils_lib)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/make-prime-list
    COMMAND ${CMAKE_C_COMPILER} ${CMAKE_SOURCE_DIR}/../src/make-prime-list.c -o ${CMAKE_BINARY_DIR}/make-prime-list
    DEPENDS ${CMAKE_SOURCE_DIR}/../src/make-prime-list.c
)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/src/primes.h
    COMMAND ${CMAKE_BINARY_DIR}/make-prime-list 5000 > ${CMAKE_BINARY_DIR}/src/primes.h
    DEPENDS ${CMAKE_BINARY_DIR}/make-prime-list
)
set_source_files_properties(../src/factor.c PROPERTIES OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/src/primes.h)

add_executable(factor
    ../src/factor.c
)
target_sources(factor PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(factor PRIVATE coreutils_lib)

add_executable(false
    ../src/false.c
)
target_sources(false PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(false PRIVATE coreutils_lib)

add_executable(fmt
    ../src/fmt.c
)
target_sources(fmt PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(fmt PRIVATE coreutils_lib)

add_executable(fold
    ../src/fold.c
)
target_sources(fold PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(fold PRIVATE coreutils_lib)

add_executable(ginstall
    ../src/install.c
    ../src/prog-fprintf.c
    ../src/copy.c
    ../src/copy-file-data.c
    ../src/cp-hash.c
    ../src/force-link.c
    ../src/selinux.c
)
target_sources(ginstall PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(ginstall PRIVATE coreutils_lib)

add_executable(groups
    ../src/groups.c
    ../src/group-list.c
)
target_sources(groups PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(groups PRIVATE coreutils_lib)

add_executable(head
    ../src/head.c
)
target_sources(head PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(head PRIVATE coreutils_lib)

add_executable(id
    ../src/id.c
    ../src/group-list.c
)
target_sources(id PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(id PRIVATE coreutils_lib)

add_executable(join
    ../src/join.c
)
target_sources(join PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(join PRIVATE coreutils_lib)

add_executable(link
    ../src/link.c
)
target_sources(link PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(link PRIVATE coreutils_lib)

add_executable(ln
    ../src/ln.c
    ../src/force-link.c
    ../src/relpath.c
)
target_sources(ln PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(ln PRIVATE coreutils_lib)

add_executable(logname
    ../src/logname.c
)
target_sources(logname PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(logname PRIVATE coreutils_lib)

add_executable(ls
    ../src/ls.c
    ../src/ls-ls.c
)
target_sources(ls PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(ls PRIVATE coreutils_lib)

add_executable(md5sum
    ../src/cksum.c
)
target_sources(md5sum PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(md5sum PRIVATE HASH_ALGO_MD5=1)
target_link_libraries(md5sum PRIVATE coreutils_lib)

add_executable(mkdir
    ../src/mkdir.c
    ../src/prog-fprintf.c
    ../src/selinux.c
)
target_sources(mkdir PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(mkdir PRIVATE coreutils_lib)

add_executable(mkfifo
    ../src/mkfifo.c
    ../src/selinux.c
)
target_sources(mkfifo PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(mkfifo PRIVATE coreutils_lib)

add_executable(mknod
    ../src/mknod.c
    ../src/selinux.c
)
target_sources(mknod PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(mknod PRIVATE coreutils_lib)

add_executable(mktemp
    ../src/mktemp.c
)
target_sources(mktemp PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(mktemp PRIVATE coreutils_lib)

add_executable(mv
    ../src/mv.c
    ../src/remove.c
    ../src/copy.c
    ../src/copy-file-data.c
    ../src/cp-hash.c
    ../src/force-link.c
    ../src/selinux.c
)
target_sources(mv PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(mv PRIVATE coreutils_lib)

add_executable(nl
    ../src/nl.c
)
target_sources(nl PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(nl PRIVATE coreutils_lib)

add_executable(nproc
    ../src/nproc.c
)
target_sources(nproc PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(nproc PRIVATE coreutils_lib)

add_executable(nohup
    ../src/nohup.c
)
target_sources(nohup PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(nohup PRIVATE coreutils_lib)

add_executable(numfmt
    ../src/numfmt.c
    ../src/set-fields.c
)
target_sources(numfmt PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(numfmt PRIVATE coreutils_lib)

add_executable(od
    ../src/od.c
)
target_sources(od PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(od PRIVATE coreutils_lib)

add_executable(paste
    ../src/paste.c
)
target_sources(paste PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(paste PRIVATE coreutils_lib)

add_executable(pathchk
    ../src/pathchk.c
)
target_sources(pathchk PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(pathchk PRIVATE coreutils_lib)

add_executable(pr
    ../src/pr.c
)
target_sources(pr PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(pr PRIVATE coreutils_lib)

add_executable(printenv
    ../src/printenv.c
)
target_sources(printenv PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(printenv PRIVATE coreutils_lib)

add_executable(printf
    ../src/printf.c
)
target_sources(printf PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(printf PRIVATE coreutils_lib)

add_executable(ptx
    ../src/ptx.c
)
target_sources(ptx PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(ptx PRIVATE coreutils_lib)

add_executable(pwd
    ../src/pwd.c
)
target_sources(pwd PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(pwd PRIVATE coreutils_lib)

add_executable(readlink
    ../src/readlink.c
)
target_sources(readlink PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(readlink PRIVATE coreutils_lib)

add_executable(realpath
    ../src/realpath.c
    ../src/relpath.c
)
target_sources(realpath PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(realpath PRIVATE coreutils_lib)

add_executable(rm
    ../src/rm.c
    ../src/remove.c
)
target_sources(rm PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(rm PRIVATE coreutils_lib)

add_executable(rmdir
    ../src/rmdir.c
    ../src/prog-fprintf.c
)
target_sources(rmdir PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(rmdir PRIVATE coreutils_lib)

add_executable(seq
    ../src/seq.c
)
target_sources(seq PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(seq PRIVATE coreutils_lib)

add_executable(sha1sum
    ../src/cksum.c
)
target_sources(sha1sum PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(sha1sum PRIVATE HASH_ALGO_SHA1=1)
target_link_libraries(sha1sum PRIVATE coreutils_lib)

add_executable(sha224sum
    ../src/cksum.c
)
target_sources(sha224sum PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(sha224sum PRIVATE HASH_ALGO_SHA224=1)
target_link_libraries(sha224sum PRIVATE coreutils_lib)

add_executable(sha256sum
    ../src/cksum.c
)
target_sources(sha256sum PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(sha256sum PRIVATE HASH_ALGO_SHA256=1)
target_link_libraries(sha256sum PRIVATE coreutils_lib)

add_executable(sha384sum
    ../src/cksum.c
)
target_sources(sha384sum PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(sha384sum PRIVATE HASH_ALGO_SHA384=1)
target_link_libraries(sha384sum PRIVATE coreutils_lib)

add_executable(sha512sum
    ../src/cksum.c
)
target_sources(sha512sum PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(sha512sum PRIVATE HASH_ALGO_SHA512=1)
target_link_libraries(sha512sum PRIVATE coreutils_lib)

add_executable(shred
    ../src/shred.c
)
target_sources(shred PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(shred PRIVATE coreutils_lib)

add_executable(shuf
    ../src/shuf.c
)
target_sources(shuf PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(shuf PRIVATE coreutils_lib)

add_executable(sleep
    ../src/sleep.c
)
target_sources(sleep PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(sleep PRIVATE coreutils_lib)

add_executable(sort
    ../src/sort.c
)
target_sources(sort PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(sort PRIVATE coreutils_lib)

add_executable(split
    ../src/split.c
    ../src/temp-stream.c
)
target_sources(split PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(split PRIVATE coreutils_lib)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/src/fs.h
    COMMAND perl ${CMAKE_SOURCE_DIR}/../src/extract-magic ${CMAKE_SOURCE_DIR}/../src/stat.c > ${CMAKE_BINARY_DIR}/src/fs.h
    DEPENDS ${CMAKE_SOURCE_DIR}/../src/extract-magic ${CMAKE_SOURCE_DIR}/../src/stat.c
)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/src/fs-is-local.h
    COMMAND perl ${CMAKE_SOURCE_DIR}/../src/extract-magic --local ${CMAKE_SOURCE_DIR}/../src/stat.c > ${CMAKE_BINARY_DIR}/src/fs-is-local.h
    DEPENDS ${CMAKE_SOURCE_DIR}/../src/extract-magic ${CMAKE_SOURCE_DIR}/../src/stat.c
)
set_source_files_properties(../src/stat.c PROPERTIES OBJECT_DEPENDS "${CMAKE_BINARY_DIR}/src/fs.h;${CMAKE_BINARY_DIR}/src/fs-is-local.h")

add_executable(stat
    ../src/stat.c
    ../src/find-mount-point.c
)
target_sources(stat PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(stat PRIVATE coreutils_lib)

add_executable(sum
    ../src/sum.c
    ../src/cksum.c
)
target_sources(sum PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_compile_definitions(sum PRIVATE HASH_ALGO_SUM=1)
target_link_libraries(sum PRIVATE coreutils_lib)

add_executable(sync
    ../src/sync.c
)
target_sources(sync PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(sync PRIVATE coreutils_lib)

add_executable(tac
    ../src/tac.c
    ../src/temp-stream.c
)
target_sources(tac PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(tac PRIVATE coreutils_lib)

add_executable(tail
    ../src/tail.c
    ../src/iopoll.c
)
target_sources(tail PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(tail PRIVATE coreutils_lib)

add_executable(tee
    ../src/tee.c
    ../src/iopoll.c
)
target_sources(tee PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(tee PRIVATE coreutils_lib)


    
    

    add_executable(cu_test
    ../src/test.c
)
target_sources(cu_test PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(cu_test PRIVATE coreutils_lib)
set_target_properties(cu_test PROPERTIES OUTPUT_NAME test)

add_executable(touch
    ../src/touch.c
)
target_sources(touch PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(touch PRIVATE coreutils_lib)

add_executable(tr
    ../src/tr.c
)
target_sources(tr PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(tr PRIVATE coreutils_lib)

add_executable(true
    ../src/true.c
)
target_sources(true PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(true PRIVATE coreutils_lib)

add_executable(truncate
    ../src/truncate.c
)
target_sources(truncate PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(truncate PRIVATE coreutils_lib)

add_executable(tsort
    ../src/tsort.c
)
target_sources(tsort PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(tsort PRIVATE coreutils_lib)

add_executable(tty
    ../src/tty.c
)
target_sources(tty PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(tty PRIVATE coreutils_lib)

add_executable(uname
    ../src/uname.c
    ../src/uname-uname.c
)
target_sources(uname PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(uname PRIVATE coreutils_lib)

add_executable(unexpand
    ../src/unexpand.c
    ../src/expand-common.c
)
target_sources(unexpand PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(unexpand PRIVATE coreutils_lib)

add_executable(uniq
    ../src/uniq.c
)
target_sources(uniq PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(uniq PRIVATE coreutils_lib)

add_executable(unlink
    ../src/unlink.c
)
target_sources(unlink PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(unlink PRIVATE coreutils_lib)

add_executable(vdir
    ../src/ls.c
    ../src/ls-vdir.c
)
target_sources(vdir PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(vdir PRIVATE coreutils_lib)

add_executable(wc
    ../src/wc.c
)
target_sources(wc PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(wc PRIVATE coreutils_lib)

add_executable(whoami
    ../src/whoami.c
)
target_sources(whoami PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(whoami PRIVATE coreutils_lib)

add_executable(yes
    ../src/yes.c
)
target_sources(yes PRIVATE $<TARGET_OBJECTS:version_obj> $<TARGET_OBJECTS:parse_datetime_obj>)
target_link_libraries(yes PRIVATE coreutils_lib)

add_test(NAME tests_help_help_version_cmake COMMAND ${CMAKE_COMMAND} -DBIN_DIR=${CMAKE_BINARY_DIR}/src "-DBUILT_PROGRAMS=arch;hostname;kill;uptime;chroot;df;hostid;nice;pinky;stdbuf;stty;timeout;users;who;chcon;runcon;lbracket;b2sum;base64;base32;basenc;basename;cat;chgrp;chmod;chown;cksum;comm;cp;csplit;cut;date;dd;dir;dircolors;dirname;du;echo;env;expand;expr;factor;false;fmt;fold;ginstall;groups;head;id;join;link;ln;logname;ls;md5sum;mkdir;mkfifo;mknod;mktemp;mv;nl;nproc;nohup;numfmt;od;paste;pathchk;pr;printenv;printf;ptx;pwd;readlink;realpath;rm;rmdir;seq;sha1sum;sha224sum;sha256sum;sha384sum;sha512sum;shred;shuf;sleep;sort;split;stat;sum;sync;tac;tail;tee;test;touch;tr;true;truncate;tsort;tty;uname;unexpand;uniq;unlink;vdir;wc;whoami;yes" -P ${CMAKE_SOURCE_DIR}/help-version.cmake)
set_tests_properties(tests_help_help_version_cmake PROPERTIES TIMEOUT 120)


# Add greatest testing framework
FetchContent_Declare(
  greatest
  GIT_REPOSITORY https://github.com/SamuelMarks/greatest.git
  GIT_TAG        master
  PATCH_COMMAND  ${CMAKE_COMMAND} -E rm -f CMakeLists.txt
)
FetchContent_MakeAvailable(greatest)

# C-based native tests using greatest
file(GLOB NATIVE_C_TEST_SCRIPTS "${CMAKE_SOURCE_DIR}/native_c_tests/test_*.c")
foreach(TEST_SRC ${NATIVE_C_TEST_SCRIPTS})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_include_directories(${TEST_NAME} PRIVATE ${greatest_SOURCE_DIR} ${CMAKE_BINARY_DIR}/lib ${CMAKE_SOURCE_DIR}/../gnulib/lib ${CMAKE_SOURCE_DIR}/lib)
    if(TARGET ${TEST_NAME})
        if(INTL_LIBRARY)
            target_link_libraries(${TEST_NAME} PRIVATE coreutils_lib ${INTL_LIBRARY})
        else()
            target_link_libraries(${TEST_NAME} PRIVATE coreutils_lib)
        endif()
    endif()
    add_test(NAME "c_${TEST_NAME}" COMMAND ${TEST_NAME} ${CMAKE_BINARY_DIR}/src)
    set_tests_properties("c_${TEST_NAME}" PROPERTIES TIMEOUT 120)
endforeach()

# Add native cmake tests
file(GLOB NATIVE_TEST_SCRIPTS "${CMAKE_SOURCE_DIR}/native_tests/*.cmake")

foreach(SCRIPT ${NATIVE_TEST_SCRIPTS})
    get_filename_component(TEST_NAME ${SCRIPT} NAME_WE)
    if(TEST_NAME MATCHES "^(test_b2sum|test_cksum|test_factor|test_fold|test_md5sum|test_sha1sum|test_sha224sum|test_sha256sum|test_sha384sum|test_sha512sum|test_sum|test_touch)$")
        continue()
    endif()
    add_test(NAME "native_${TEST_NAME}" COMMAND ${CMAKE_COMMAND} -DBIN_DIR=${CMAKE_BINARY_DIR}/src -P ${SCRIPT})
    set_tests_properties("native_${TEST_NAME}" PROPERTIES TIMEOUT 120)
endforeach()

get_property(ALL_TESTS DIRECTORY PROPERTY TESTS)

set_property(TEST ${ALL_TESTS} PROPERTY SKIP_RETURN_CODE 77)
add_executable(getlimits ../src/getlimits.c)
if(INTL_LIBRARY)
    target_link_libraries(getlimits PRIVATE coreutils_lib ${INTL_LIBRARY})
else()
    target_link_libraries(getlimits PRIVATE coreutils_lib)
endif()
set_target_properties(libstdbuf PROPERTIES PREFIX "" SUFFIX ".so")

file(APPEND ${CMAKE_BINARY_DIR}/lib/config.h "
#if defined(__APPLE__) || (defined(__linux__) && !defined(__GLIBC__))
#define off64_t off_t
#endif
")
# Added test_misc.c implicitly by glob, wait, the glob will find it!
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lacl -lcap")
endif()

get_property(targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
foreach(target ${targets})
    get_target_property(target_type ${target} TYPE)
    if(target_type STREQUAL "EXECUTABLE")
        target_compile_definitions(${target} PRIVATE COMPILING_SRC)
        target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/../gnulib/lib)
    endif()
endforeach()
file(APPEND ${CMAKE_BINARY_DIR}/lib/config.h "
#ifndef fgetfilecon_raw
# define fgetfilecon_raw fgetfilecon
#endif
#ifndef setfilecon_raw
# define setfilecon_raw setfilecon
#endif
#ifndef fsetfilecon_raw
# define fsetfilecon_raw fsetfilecon
#endif
#ifndef getfilecon_raw
# define getfilecon_raw getfilecon
#endif
#ifndef selabel_lookup_raw
# define selabel_lookup_raw selabel_lookup
#endif
#ifndef lsetfilecon_raw
# define lsetfilecon_raw lsetfilecon
#endif
")
file(APPEND ${CMAKE_BINARY_DIR}/lib/config.h "
#undef HAVE_STROPTS_H
#ifndef mbsstr
extern char *mbsstr (const char *haystack, const char *needle);
#endif
#ifndef mbs_endswith
extern bool mbs_endswith (const char *string, const char *suffix);
#endif
extern const char *strerrorname_np (int errnum);
")
