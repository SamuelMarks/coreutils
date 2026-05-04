#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



static void cleanup() {
    unlink("file");
    unlink("copy1");
    unlink("copy2");
}

TEST test_cp_capability(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("Capabilities not supported on Windows");
#endif

    // check if we are root
    if (geteuid() != 0) {
        SKIPm("must be run as root");
    }

    FILE *f = fopen("file", "w");
    if (f) {
        fprintf(f, "data\n");
        if (f) fclose(f);
    }

    // Try to setcap. If it fails, skip.
    if (my_system("setcap 'cap_net_bind_service=ep' file >/dev/null 2>&1") != 0) {
        cleanup();
        SKIPm("setcap doesn't work or not found");
    }

    char out[1024] = {0};
    if (my_system("getcap file > getcap_out 2>/dev/null") != 0) {
        cleanup();
        unlink("getcap_out");
        SKIPm("getcap doesn't work or not found");
    }
    
    FILE *cap_f = fopen("getcap_out", "r");
    if (cap_f) {
        fread(out, 1, sizeof(out)-1, cap_f);
        fclose(cap_f);
    }
    unlink("getcap_out");

    if (strstr(out, "cap_net_bind_service") == NULL) {
        cleanup();
        SKIPm("getcap did not report expected capability");
    }

    ASSERT_EQ(0, exec_capture("cp --preserve=xattr file copy1", NULL, 0));
    ASSERT_EQ(0, exec_capture("cp --preserve=all file copy2", NULL, 0));

    const char *copies[] = {"copy1", "copy2"};
    for (int i = 0; i < 2; i++) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "getcap %s > getcap_out 2>/dev/null", copies[i]);
        ASSERT_EQ(0, my_system(cmd));
        
        char out2[1024] = {0};
        FILE *cf = fopen("getcap_out", "r");
        ASSERT(cf != NULL);
        fread(out2, 1, sizeof(out2)-1, cf);
        fclose(cf);
        unlink("getcap_out");

        ASSERT(strstr(out2, "cap_net_bind_service") != NULL);
    }

    cleanup();
    PASS();
}





SUITE(cp_capability_suite) {
    RUN_TEST(test_cp_capability);
}
DEFINE_TEST_MAIN(cp_capability_suite)
