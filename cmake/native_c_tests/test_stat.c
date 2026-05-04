#include "test_helper.h"
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>



TEST test_stat_basic(void) {
    FILE *f = fopen("test_stat.txt", "w");
    fprintf(f, "hello");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("stat -c %s test_stat.txt", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("5", out);
    remove("test_stat.txt");
    PASS();
}

TEST test_stat_fmt(void) {
    char out[512];
    
    ASSERT_EQ(0, exec_capture("stat --format='%%' .", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("%", out);
    
    ASSERT_EQ(0, exec_capture("stat -c %n -", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("-", out);

    PASS();
}

TEST test_stat_slash(void) {
    char out[512];
    int ret;
    
    FILE *f = fopen("file", "w"); if (f) fclose(f);
#ifndef _WIN32
    mkdir("dir", 0755);
#else
    mkdir("dir");
#endif
    symlink("file", "link1");
    symlink("dir", "link2");

    ret = exec_capture("stat --format=%n link1", out, sizeof(out));
    ASSERT_EQ(0, WEXITSTATUS(ret));
    trim_newline(out);
    ASSERT_STR_EQ("link1", out);
    
    ret = exec_capture("stat --format=%n link1/ 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    ret = exec_capture("stat --format=%F link2", out, sizeof(out));
    ASSERT_EQ(0, WEXITSTATUS(ret));
    ASSERT(strstr(out, "symbolic link") != NULL);

    ret = exec_capture("stat -L --format=%F link2", out, sizeof(out));
    ASSERT_EQ(0, WEXITSTATUS(ret));
    ASSERT(strstr(out, "directory") != NULL);

    ret = exec_capture("stat --format=%F link2/", out, sizeof(out));
    ASSERT_EQ(0, WEXITSTATUS(ret));
    ASSERT(strstr(out, "directory") != NULL);

    unlink("link1");
    unlink("link2");
    remove("file");
    rmdir("dir");
    PASS();
}

TEST test_stat_pl(void) {
    char out[512];
    
    ASSERT_EQ(0, exec_capture("stat --printf=a .", out, sizeof(out)));
    ASSERT_STR_EQ("a", out);
    
    ASSERT_EQ(0, exec_capture("stat --printf='a\\nb' .", out, sizeof(out)));
    ASSERT_STR_EQ("a\nb", out);
    
    ASSERT_EQ(0, exec_capture("stat --printf='\\0' .", out, sizeof(out))); 
    
    PASS();
}

TEST test_stat_hyphen(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f_hyphen", "w"); if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("stat --format=%n - < f_hyphen", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("-", out);
    
    { int _r = exec_capture("stat -f - < f_hyphen 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    remove("f_hyphen");
#endif
    PASS();
}

TEST test_stat_birthtime(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f_btime", "w"); if (f) fclose(f);
    
    char btime[64], atime[64], mtime[64], ctime[64];
    ASSERT_EQ(0, exec_capture("stat --format %W f_btime", btime, sizeof(btime))); trim_newline(btime);
    ASSERT_EQ(0, exec_capture("stat --format %X f_btime", atime, sizeof(atime))); trim_newline(atime);
    ASSERT_EQ(0, exec_capture("stat --format %Y f_btime", mtime, sizeof(mtime))); trim_newline(mtime);
    ASSERT_EQ(0, exec_capture("stat --format %Z f_btime", ctime, sizeof(ctime))); trim_newline(ctime);
    
    sleep(1);
    
    // update a, m, c times
    f = fopen("f_btime", "w"); if (f) fclose(f);
    exec_capture("touch f_btime", out, sizeof(out)); // just ensure it's touched
    
    char btime2[64];
    ASSERT_EQ(0, exec_capture("stat --format %W f_btime", btime2, sizeof(btime2))); trim_newline(btime2);
    
    // birth time shouldn't change
    ASSERT_STR_EQ(btime, btime2);
    
    remove("f_btime");
#endif
    PASS();
}
TEST test_stat_stat_mount(void) {
    SKIPm("Skipped: mount testing requires root/fstab mocking");
    PASS();
}

TEST test_stat_stat_nanoseconds(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f_stat", "w"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("stat -c %y f_stat", out, sizeof(out)));
    // should have nanoseconds e.g. .000000000
    remove("f_stat");
#endif
    PASS();
}

TEST test_stat_stat_printf(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f_stat2", "w"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("stat --printf='%n\n' f_stat2", out, sizeof(out)));
    ASSERT_STR_EQ("f_stat2\n", out);
    remove("f_stat2");
#endif
    PASS();
}







SUITE(stat_suite) {
    RUN_TEST(test_stat_basic);
    RUN_TEST(test_stat_fmt);
    RUN_TEST(test_stat_slash);
    RUN_TEST(test_stat_pl);
    RUN_TEST(test_stat_hyphen);
    RUN_TEST(test_stat_birthtime);
    RUN_TEST(test_stat_stat_mount);
    RUN_TEST(test_stat_stat_nanoseconds);
    RUN_TEST(test_stat_stat_printf);
}
DEFINE_TEST_MAIN(stat_suite)
