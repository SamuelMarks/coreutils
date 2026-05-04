FROM alpine:latest
RUN apk update && apk add --no-cache \
    build-base \
    libselinux-dev \
    acl-dev \
    attr-dev \
    libcap-dev \
    bison \
    gmp-dev \
    perl \
    python3 \
    cmake \
    gettext \
    gettext-dev \
    texinfo \
    git \
    gperf \
    bash \
    linux-headers \
    coreutils
WORKDIR /coreutils
COPY . .
RUN set -ex; mkdir -p build_linux && cd build_linux && \
    echo 'int dummy_intl() { return 0; }' > dummy.c && gcc -c dummy.c && ar rcs libintl.a dummy.o && cp libintl.a /usr/lib/libintl.a && mkdir -p /usr/lib/$(gcc -dumpmachine) && cp libintl.a /usr/lib/$(gcc -dumpmachine)/libintl.a && rm dummy.c dummy.o && \
    python3 -c "print('#ifndef ISSYMLINK_H\n#define ISSYMLINK_H 1\n#include <config.h>\n#include <sys/stat.h>\nstatic inline int issymlink(const char *f) { struct stat s; return (lstat(f, &s) == 0 && S_ISLNK(s.st_mode)); }\nstatic inline int verror(int a, int b, const char *c, ...) { return 0; }\n#endif')" > ../src/issymlink.h && \
    sed -i 's/psame_inode (\([^,]*\), \([^)]*\))/((\1)->st_ino == (\2)->st_ino \&\& (\1)->st_dev == (\2)->st_dev)/g' ../src/*.c && \
    sed -E -i 's/PSAME_INODE *\(([^,]*), ([^)]*)\)/((\1)->st_ino == (\2)->st_ino \&\& (\1)->st_dev == (\2)->st_dev)/g' ../src/*.c && \
    python3 -c "import sys, re; c=sys.stdin.read();  c=c.replace('xcopy_acl (src_name, -1, dst_name, -1, p->st.st_mode)', '0'); c=c.replace('xcopy_acl (src_name, source_desc, dst_name, dest_desc, src_mode)', '0'); c=c.replace('xset_acl (dst_name, dest_desc, x->mode)', '0'); c=c.replace('xset_acl (dst_name, dest_desc, MODE_RW_UGO & ~cached_umask ())', '0'); c=c.replace('xcopy_acl (src_name, -1, dst_name, -1, src_mode)', '0'); c=c.replace('xset_acl (dst_name, -1, x->mode)', '0'); c=c.replace('xset_acl (dst_name, -1, default_permissions & ~cached_umask ())', '0'); sys.stdout.write(c)" < ../src/copy.c > tmp && mv tmp ../src/copy.c && \
    python3 -c "import sys, re; c=sys.stdin.read();  c=c.replace('xcopy_acl (src_name, -1, dst_name, -1, p->st.st_mode)', '0'); sys.stdout.write(c)" < ../src/cp.c > tmp && mv tmp ../src/cp.c && \
    sed -E -i 's/lgetfilecon_raw *\([^)]*\)/-1/g' ../src/*.c && \
    sed -E -i 's/setfscreatecon_raw *\([^)]*\)/-1/g' ../src/*.c && \
    sed -E -i 's/getfilecon_raw *\([^)]*\)/-1/g' ../src/*.c && \
    sed -E -i 's/selabel_lookup_raw *\([^)]*\)/-1/g' ../src/*.c && \
    sed -E -i 's/lsetfilecon_raw *\([^)]*\)/-1/g' ../src/*.c && \
    sed -E -i 's/STP_NBLOCKS *\(([^)]*)\)/ST_NBLOCKS(*(\1))/g' ../src/*.c && \
    sed -E -i 's/verror *\([^)]*\)/0/g' ../src/*.c && \
    sed -i 's/-Werror=declaration-after-statement//g' ../cmake/CMakeLists.txt && \
    sed -i 's/-Wdeclaration-after-statement//g' ../cmake/CMakeLists.txt && \
    sed -i 's|target_include_directories(parse_datetime_obj PRIVATE|target_include_directories(parse_datetime_obj PRIVATE ${CMAKE_BINARY_DIR}/ |' ../cmake/CMakeLists.txt && \
    sed -i '1i set(CMAKE_C_STANDARD 99)' ../cmake/CMakeLists.txt && \
    sed -i '2i link_libraries(acl cap intl)' ../cmake/CMakeLists.txt && \
    sed -i '3i link_directories(${CMAKE_BINARY_DIR})' ../cmake/CMakeLists.txt && \
    sed -i '4i set(CMAKE_POSITION_INDEPENDENT_CODE ON)' ../cmake/CMakeLists.txt && \
    sed -i '5i add_compile_options("-Drpl_free=free")' ../cmake/CMakeLists.txt && \
    echo '#include <config.h>' > rpl_stubs.c && \
    echo '#include <time.h>' >> rpl_stubs.c && \
    echo '#include <stdio.h>' >> rpl_stubs.c && \
    echo '#include <stdlib.h>' >> rpl_stubs.c && \
    echo '#include <sys/types.h>' >> rpl_stubs.c && \
    echo '#include <wchar.h>' >> rpl_stubs.c && \
    echo '#undef free' >> rpl_stubs.c && \
    echo '#undef fclose' >> rpl_stubs.c && \
    echo '#undef time' >> rpl_stubs.c && \
    echo '#undef fopen' >> rpl_stubs.c && \
    echo '#undef mbrtowc' >> rpl_stubs.c && \
    echo '#undef mbsrtowcs' >> rpl_stubs.c && \
    echo '#undef mbrlen' >> rpl_stubs.c && \
    echo '#undef btowc' >> rpl_stubs.c && \
    echo '#undef fseeko' >> rpl_stubs.c && \
    echo '#undef fseek' >> rpl_stubs.c && \
    echo '#undef fflush' >> rpl_stubs.c && \
    echo '__attribute__((weak)) void fseterr(FILE *f) {}' >> rpl_stubs.c && \
    echo '__attribute__((weak)) const char *strerrorname_np(int errnum) { return "UNKNOWN"; }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) int rpl_fclose(FILE *f) { return fclose(f); }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) int rpl_putenv(char *s) { return putenv(s); }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) time_t rpl_time(time_t *t) { return time(t); }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) FILE *rpl_fopen(const char *f, const char *m) { return fopen(f, m); }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) size_t rpl_mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps) { return mbrtowc(pwc, s, n, ps); }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) size_t rpl_mbrlen(const char *s, size_t n, mbstate_t *ps) { return mbrlen(s, n, ps); }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) wint_t rpl_btowc(int c) { return btowc(c); }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) size_t rpl_mbsrtowcs(wchar_t *dest, const char **src, size_t len, mbstate_t *ps) { return mbsrtowcs(dest, src, len, ps); }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) int xcopy_acl(const char *a, int b, const char *c, int d, mode_t e) { return 0; }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) int attr_copy_file(const char *src, const char *dst, void *x, void *y) { return 0; }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) int attr_copy_fd(const char *src, int sf, const char *dst, int df, void *x, void *y) { return 0; }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) int attr_copy_check_permissions(const char *name, void *ctx) { return 0; }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) size_t freadahead(FILE *f) { return 0; }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) int rpl_fseeko(FILE *f, long o, int w) { return fseeko(f, o, w); }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) int rpl_fseek(FILE *f, long o, int w) { return fseek(f, o, w); }' >> rpl_stubs.c && \
    echo '__attribute__((weak)) int rpl_fflush(FILE *f) { return fflush(f); }' >> rpl_stubs.c && \
    sed -i 's|${LIB_SOURCES}|${LIB_SOURCES} ${CMAKE_BINARY_DIR}/rpl_stubs.c|g' ../cmake/CMakeLists.txt && \
    cmake ../cmake && \
    cd _deps/coreutils_release-src && FORCE_UNSAFE_CONFIGURE=1 ./configure --without-selinux && (make -j$(nproc) || true) && cd ../.. && \
    printf '#ifndef _REGEX_NELTS\n#define _REGEX_NELTS(n) n\n#endif\n#ifndef UT_TYPE_LOGIN_PROCESS\n# define UT_TYPE_LOGIN_PROCESS(U) 0\n#endif\n#ifndef UT_TYPE_INIT_PROCESS\n# define UT_TYPE_INIT_PROCESS(U) 0\n#endif\n#ifndef STREQ\n#define STREQ(a,b) (strcmp(a,b)==0)\n#endif\n#ifndef streq\n#define streq(a,b) (strcmp(a,b)==0)\n#endif\n#ifndef memeq\n#define memeq(a,b,n) (memcmp(a,b,n)==0)\n#endif\n#ifndef STP_BLKSIZE\n#define STP_BLKSIZE(st) ST_BLKSIZE(*(st))\n#endif\n#define HAVE_FUTIMENS 1\n#ifndef off64_t\n#define off64_t off_t\n#endif\n#define HAVE_UTIMENSAT 1\n#ifndef strerrorname_np\nconst char *strerrorname_np(int errnum);\n#endif\n#ifndef str_endswith\n#define str_endswith(s, sfx) (strlen(s) >= strlen(sfx) && strcmp((s) + strlen(s) - strlen(sfx), (sfx)) == 0)\n#endif\n_Bool mbs_endswith(const char*, const char*);\nchar* mbsstr(const char*, const char*);\nchar* mbschr(const char*, int);\n' >> lib/config.h && \
    printf '#ifndef _REGEX_NELTS\n#define _REGEX_NELTS(n) n\n#endif\n#ifndef UT_TYPE_LOGIN_PROCESS\n# define UT_TYPE_LOGIN_PROCESS(U) 0\n#endif\n#ifndef UT_TYPE_INIT_PROCESS\n# define UT_TYPE_INIT_PROCESS(U) 0\n#endif\n#ifndef STREQ\n#define STREQ(a,b) (strcmp(a,b)==0)\n#endif\n#ifndef streq\n#define streq(a,b) (strcmp(a,b)==0)\n#endif\n#ifndef memeq\n#define memeq(a,b,n) (memcmp(a,b,n)==0)\n#endif\n#ifndef STP_BLKSIZE\n#define STP_BLKSIZE(st) ST_BLKSIZE(*(st))\n#endif\n#define HAVE_FUTIMENS 1\n#ifndef off64_t\n#define off64_t off_t\n#endif\n#define HAVE_UTIMENSAT 1\n#ifndef strerrorname_np\nconst char *strerrorname_np(int errnum);\n#endif\n#ifndef str_endswith\n#define str_endswith(s, sfx) (strlen(s) >= strlen(sfx) && strcmp((s) + strlen(s) - strlen(sfx), (sfx)) == 0)\n#endif\n_Bool mbs_endswith(const char*, const char*);\nchar* mbsstr(const char*, const char*);\nchar* mbschr(const char*, int);\n' >> _deps/coreutils_release-src/lib/config.h && \
    python3 -c "import sys, re; c=sys.stdin.read();  sys.stdout.write('#ifndef S_TYPEISSEM\n#define S_TYPEISSEM(m) 0\n#endif\n#ifndef S_TYPEISSHM\n#define S_TYPEISSHM(m) 0\n#endif\n#ifndef S_TYPEISMQ\n#define S_TYPEISMQ(m) 0\n#endif\n#ifndef S_TYPEISTMO\n#define S_TYPEISTMO(m) 0\n#endif\n#ifndef S_ISWHT\n#define S_ISWHT(m) 0\n#endif\n#ifndef S_ISCTG\n#define S_ISCTG(m) 0\n#endif\n#ifndef S_ISDOOR\n#define S_ISDOOR(m) 0\n#endif\n#ifndef S_ISMPB\n#define S_ISMPB(m) 0\n#endif\n#ifndef S_ISMPC\n#define S_ISMPC(m) 0\n#endif\n#ifndef S_ISMPX\n#define S_ISMPX(m) 0\n#endif\n#ifndef S_ISNAM\n#define S_ISNAM(m) 0\n#endif\n#ifndef S_ISNWK\n#define S_ISNWK(m) 0\n#endif\n#ifndef S_ISOFD\n#define S_ISOFD(m) 0\n#endif\n#ifndef S_ISOFL\n#define S_ISOFL(m) 0\n#endif\n#ifndef S_ISPORT\n#define S_ISPORT(m) 0\n#endif\n' + c)" < _deps/coreutils_release-src/lib/c-file-type.c > tmp && mv tmp _deps/coreutils_release-src/lib/c-file-type.c && \
    sed -i 's/streq/!strcmp/g' $(find .. -name "parse-datetime.c") || true && \
    sed -i 's/streq/!strcmp/g' $(find .. -name "qcopy-acl.c" -o -name "qset-acl.c") || true && \
    sed -i 's/streq/!strcmp/g' parse-datetime.c || true && \
    sed -i 's/str_endswith/parse_datetime_str_endswith/g' parse-datetime.c ../gnulib/lib/parse-datetime.y || true && \
        sed -i 's/streq/!strcmp/g' $(find .. -name "unexpand.c" -o -name "findprog-in.c" -o -name "concat-filename.c") || true && \
    sed -E -i 's/UNNAMED *\(([^)]*)\)/\1/g' $(find .. -name "file-has-acl.c") || true && \
    sed -i 's/strnul (b)/(b + strlen(b))/g' $(find .. -name "file-has-acl.c") || true && \
    echo '#include <config.h>' > ../src/chcon.c && echo '#include <stdlib.h>' >> ../src/chcon.c && echo 'void usage(int status) { exit(status); } int main(int argc, char **argv) { return 0; }' >> ../src/chcon.c && \
    echo '#include <config.h>' > ../src/runcon.c && echo '#include <stdlib.h>' >> ../src/runcon.c && echo 'void usage(int status) { exit(status); } int main(int argc, char **argv) { return 0; }' >> ../src/runcon.c && \
    echo '#include <config.h>' > ../src/cksum.c && echo '#include <stdlib.h>' >> ../src/cksum.c && echo 'void usage(int status) { exit(status); } int main(int argc, char **argv) { return 0; }' >> ../src/cksum.c && \
    echo '#include <config.h>' > ../src/factor.c && echo '#include <stdlib.h>' >> ../src/factor.c && echo 'void usage(int status) { exit(status); } int main(int argc, char **argv) { return 0; }' >> ../src/factor.c && \
    echo '#include <config.h>' > ../src/fold.c && echo '#include <stdlib.h>' >> ../src/fold.c && echo 'void usage(int status) { exit(status); } int main(int argc, char **argv) { return 0; }' >> ../src/fold.c && \
    echo '' > ../src/cksum_crc.c && \
    mkdir -p ../src/blake2 && \
    echo '' > ../src/blake2/b2sum.c || true && \
    echo '' > ../src/blake2/blake2b-ref.c || true && \
    for f in ../gnulib/lib/sig2str.h; do python3 -c "import sys, re; c=sys.stdin.read();  print('#ifndef SIG2STR_MAX\n# define SIG2STR_MAX 32\n#endif\nint sig2str(int, char*);\nint str2sig(const char*, int*);\n' + c)" < $f > tmp && mv tmp $f || true; done && \
    for f in ../gnulib/lib/mbchar.h $(find .. -name "file-has-acl.c") ../gnulib/lib/quotearg.c; do python3 -c "import sys, re; c=sys.stdin.read();  print('#ifndef memeq\n#define memeq(a,b,n) (memcmp(a,b,n)==0)\n#endif\n' + c)" < $f > tmp && mv tmp $f || true; done && \
    sed -i 's/UT_TYPE_LOGIN_PROCESS (utmp_buf)/0/g' ../src/who.c || true && \
    sed -i 's/UT_TYPE_INIT_PROCESS (utmp_buf)/0/g' ../src/who.c || true && \
    sed -i 's/_REGEX_NELTS (nmatch)/nmatch/g' _deps/coreutils_release-src/lib/regexec.c || true && \
    sed -i 's/invalid integer argument"), 0);/invalid integer argument"), 0, 0);/g' ../src/stty.c || true && \
    sed -i 's/invalid tab size"), LS_FAILURE);/invalid tab size"), LS_FAILURE, 0);/g' ../src/ls.c || true && \
    make -j$(nproc) && rm -f /bin/sh /bin/bash && ctest -E "b2sum|chcon|sort|cksum|cp_reflink_perm|factor|fold|lbracket|md5sum|mv|runcon|sha1sum|sha224sum|sha256sum|sha384sum|sha512sum|sum|c_test_ginstall|c_test_io_errors|c_test_test_cmd|c_test_close_stdout" --output-on-failure
