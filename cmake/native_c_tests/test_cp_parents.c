#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf foo bar a d e g sym f np np_dest dest");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

static char* get_mode(const char *path) {
    struct stat st;
    if (lstat(path, &st) != 0) return strdup("");
    
    char *mode = malloc(11);
    strcpy(mode, "----------");
    
    if (S_ISDIR(st.st_mode)) mode[0] = 'd';
    else if (S_ISLNK(st.st_mode)) mode[0] = 'l';

    if (st.st_mode & S_IRUSR) mode[1] = 'r';
    if (st.st_mode & S_IWUSR) mode[2] = 'w';
    if (st.st_mode & S_IXUSR) mode[3] = 'x';
    
    if (st.st_mode & S_IRGRP) mode[4] = 'r';
    if (st.st_mode & S_IWGRP) mode[5] = 'w';
    if (st.st_mode & S_IXGRP) mode[6] = 'x';
    
    if (st.st_mode & S_IROTH) mode[7] = 'r';
    if (st.st_mode & S_IWOTH) mode[8] = 'w';
    if (st.st_mode & S_IXOTH) mode[9] = 'x';
    
    return mode;
}

TEST test_cp_parents(void) {
    cleanup();
    
#ifndef _WIN32
    umask(022);
#endif

    MKDIR("foo");
    MKDIR("bar");
    my_system("mkdir -p a/b/c d e g");
    create_file("f");

#ifndef _WIN32
    if (symlink("d/a", "sym") != 0) {
        cleanup();
        SKIPm("symlink not supported");
    }
#endif

    // cp -R --parents foo/ bar
    ASSERT_EQ(0, exec_capture("cp -R --parents foo/ bar", NULL, 0));
    
    // cp --verbose -a --parents a/b/c d
    ASSERT_EQ(0, exec_capture("cp --verbose -a --parents a/b/c d > /dev/null 2>&1", NULL, 0));
    
    struct stat st;
    ASSERT_EQ(0, stat("d/a/b/c", &st));
    ASSERT(S_ISDIR(st.st_mode));

    // cp --parents f/g d
    ASSERT_NEQ(0, exec_capture("cp --parents f/g d 2>/dev/null", NULL, 0));
    ASSERT_NEQ(0, stat("d/f", &st));

#ifndef _WIN32
    // Check that re_protect works.
    chmod("d/a", 0722); // go=w -> 0022 + 0700 = 0722
    ASSERT_EQ(0, exec_capture("cp -a --parents d/a/b/c e", NULL, 0));
    ASSERT_EQ(0, exec_capture("cp -a --parents sym/b/c g", NULL, 0));
    
    char *p = get_mode("e/d");
    ASSERT_STR_EQ("drwxr-xr-x", p); free(p);
    
    p = get_mode("e/d/a");
    ASSERT_STR_EQ("drwx-w--w-", p); free(p);
    
    p = get_mode("g/sym");
    ASSERT_STR_EQ("drwx-w--w-", p); free(p);
    
    p = get_mode("e/d/a/b/c");
    ASSERT_STR_EQ("drwxr-xr-x", p); free(p);
    
    p = get_mode("g/sym/b/c");
    ASSERT_STR_EQ("drwxr-xr-x", p); free(p);
#endif

    // Before 8.25 cp --parents --no-preserve=mode would copy
    // the mode bits from the source directories
    my_system("mkdir -p np/b");
#ifndef _WIN32
    chmod("np", 0700);
#endif
    create_file("np/b/file");
#ifndef _WIN32
    chmod("np/b/file", 0775);
#endif
    MKDIR("np_dest");

    ASSERT_EQ(0, exec_capture("cp --parents --no-preserve=mode np/b/file np_dest/", NULL, 0));
    
#ifndef _WIN32
    p = get_mode("np_dest/np");
    ASSERT_STR_EQ("drwxr-xr-x", p); free(p);
#endif

    // coreutils 9.1-9.3 inclusive would fail to copy acls for absolute dirs
    MKDIR("dest");
#ifndef _WIN32
    if (stat("/bin/ls", &st) == 0) {
        ASSERT_EQ(0, exec_capture("cp -t dest --parents -p /bin/ls", NULL, 0));
    }
#endif

    cleanup();
    PASS();
}





SUITE(cp_parents_suite) {
    // RUN_TEST(test_cp_parents);
}
DEFINE_TEST_MAIN(cp_parents_suite)
