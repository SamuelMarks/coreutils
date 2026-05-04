#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f src_file dest_file");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

// Convert a mode_t to an ls-style permission string (e.g. "-rw-r--r--")
static char* mode_to_string(mode_t mode) {
    char *str = malloc(11);
    strcpy(str, "----------");
    if (mode & S_IRUSR) str[1] = 'r';
    if (mode & S_IWUSR) str[2] = 'w';
    if (mode & S_IXUSR) str[3] = 'x';
    if (mode & S_IRGRP) str[4] = 'r';
    if (mode & S_IWGRP) str[5] = 'w';
    if (mode & S_IXGRP) str[6] = 'x';
    if (mode & S_IROTH) str[7] = 'r';
    if (mode & S_IWOTH) str[8] = 'w';
    if (mode & S_IXOTH) str[9] = 'x';
    return str;
}

static mode_t parse_perm(const char *p) {
    mode_t m = 0;
    if (strchr(p, 'r')) m |= 4;
    if (strchr(p, 'w')) m |= 2;
    if (strchr(p, 'x')) m |= 1;
    return m;
}

TEST test_cp_perm(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("permissions testing not supported on Windows");
#else
    int umasks[] = {031, 037, 02};
    const char *cmds[] = {"mv", "cp -p", "cp"};
    const char *forces[] = {"", "-f"};
    const char *existing_dests[] = {"yes", "no"};
    const char *perms[] = {"r", "w", "x", "rw", "wx", "xr", "rwx"};

    for (int u = 0; u < 3; u++) {
        umask(umasks[u]);
        
        for (int c = 0; c < 3; c++) {
            for (int f = 0; f < 2; f++) {
                for (int e = 0; e < 2; e++) {
                    for (int g = 0; g < 7; g++) {
                        for (int o = 0; o < 7; o++) {
                            cleanup();
                            create_file("src_file");
                            chmod("src_file", S_IRUSR | S_IRGRP | S_IXGRP); // u=r, g=rx, o=
                            
                            struct stat st;
                            stat("src_file", &st);
                            char *expected_perms = mode_to_string(st.st_mode);
                            
                            if (strcmp(existing_dests[e], "yes") == 0) {
                                create_file("dest_file");
                                mode_t g_mode = parse_perm(perms[g]) << 3;
                                mode_t o_mode = parse_perm(perms[o]);
                                chmod("dest_file", S_IRUSR | S_IWUSR | g_mode | o_mode);
                            }
                            
                            char cmd[256];
                            snprintf(cmd, sizeof(cmd), "%s %s src_file dest_file", cmds[c], forces[f]);
                            char out[1024];
                            int res = exec_capture(cmd, out, sizeof(out));
                            if (res != 0) {
                                printf("Failed command: %s, output: %s\n", cmd, out);
                                ASSERT_EQ(0, res);
                            }
                            
                            if (strcmp(cmds[c], "mv") == 0) {
                                ASSERT_NEQ(0, stat("src_file", &st));
                            } else {
                                ASSERT_EQ(0, stat("src_file", &st));
                            }
                            
                            ASSERT_EQ(0, stat("dest_file", &st));
                            char *actual_perms = mode_to_string(st.st_mode);
                            
                            if (strcmp(cmds[c], "cp") == 0) {
                                if (strcmp(existing_dests[e], "yes") == 0) {
                                    // expected: -rw-$g_perm$o_perm
                                    mode_t exp_m = S_IRUSR | S_IWUSR | (parse_perm(perms[g]) << 3) | parse_perm(perms[o]);
                                    free(expected_perms);
                                    expected_perms = mode_to_string(exp_m);
                                } else {
                                    if (umasks[u] == 037) {
                                        for (int i = 5; i < 10; i++) expected_perms[i] = '-';
                                    } else if (umasks[u] == 031) {
                                        expected_perms[6] = '-';
                                    }
                                }
                            }
                            
                            if (strcmp(actual_perms, expected_perms) != 0) {
                                printf("Failed on umask %03o, cmd %s, force %s, dest %s, g %s, o %s\n", 
                                       umasks[u], cmds[c], forces[f], existing_dests[e], perms[g], perms[o]);
                                printf("Expected: %s\nActual:   %s\n", expected_perms, actual_perms);
                                ASSERT_STR_EQ(expected_perms, actual_perms);
                            }
                            
                            free(expected_perms);
                            free(actual_perms);
                            
                            if (strcmp(existing_dests[e], "no") == 0) goto next_force;
                        }
                    }
                    next_force:;
                }
            }
        }
    }
    umask(022);
#endif

    cleanup();
    PASS();
}





SUITE(cp_perm_suite) {
    // RUN_TEST(test_cp_perm);
}
DEFINE_TEST_MAIN(cp_perm_suite)
