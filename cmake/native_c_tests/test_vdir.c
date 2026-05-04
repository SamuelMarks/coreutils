#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif




#ifdef _WIN32
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0777)
#endif

TEST test_vdir_basic(void) {
    MKDIR("test_vdir_dir");
    FILE *f = fopen("test_vdir_dir/file1.txt", "w");
    fprintf(f, "data");
    if (f) fclose(f);
    
    char out[512];
    ASSERT_EQ(0, exec_capture("vdir test_vdir_dir", out, sizeof(out)));
    
    ASSERT(strstr(out, "file1.txt") != NULL);
    
    remove("test_vdir_dir/file1.txt");
    exec_capture("rmdir test_vdir_dir", NULL, 0);
    PASS();
}



SUITE(vdir_suite) {
    RUN_TEST(test_vdir_basic);
}
DEFINE_TEST_MAIN(vdir_suite)
