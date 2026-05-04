#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>



static void cleanup() {
    my_system("rm -f x y y~ y.~1~ y.~2~ y.~3~");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

// Compare directories and files
static int cmp_strings(const void *a, const void *b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

static char* ls_xy() {
    DIR *d = opendir(".");
    if (!d) return strdup("");
    struct dirent *dir;
    char *files[100];
    int count = 0;
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] == 'x' || dir->d_name[0] == 'y') {
            files[count++] = strdup(dir->d_name);
        }
    }
    closedir(d);
    qsort(files, count, sizeof(char*), cmp_strings);
    
    char buf[1024] = {0};
    for (int i = 0; i < count; i++) {
        if (i > 0) strcat(buf, " ");
        strcat(buf, files[i]);
        free(files[i]);
    }
    return strdup(buf);
}

TEST test_cp_mv_backup(void) {
    cleanup();

    const char *progs[] = {"cp", "mv"};
    const char *initial_files_cases[] = {"x", "x y", "x y y~", "x y y.~1~", "x y y~ y.~1~"};
    const char *opts[] = {"none", "off", "numbered", "t", "existing", "nil", "simple", "never"};

    char expected_buf[8192] = {0};
    char actual_buf[8192] = {0};

    // Build the expected string programmatically based on the shell script logic
    const char *expected_lines[] = {
        "x none: x y", "x off: x y", "x numbered: x y", "x t: x y", "x existing: x y", "x nil: x y", "x simple: x y", "x never: x y",
        "x y none: x y", "x y off: x y", "x y numbered: x y y.~1~", "x y t: x y y.~1~", "x y existing: x y y~", "x y nil: x y y~", "x y simple: x y y~", "x y never: x y y~",
        "x y y~ none: x y y~", "x y y~ off: x y y~", "x y y~ numbered: x y y.~1~ y~", "x y y~ t: x y y.~1~ y~", "x y y~ existing: x y y~", "x y y~ nil: x y y~", "x y y~ simple: x y y~", "x y y~ never: x y y~",
        "x y y.~1~ none: x y y.~1~", "x y y.~1~ off: x y y.~1~", "x y y.~1~ numbered: x y y.~1~ y.~2~", "x y y.~1~ t: x y y.~1~ y.~2~", "x y y.~1~ existing: x y y.~1~ y.~2~", "x y y.~1~ nil: x y y.~1~ y.~2~", "x y y.~1~ simple: x y y.~1~ y~", "x y y.~1~ never: x y y.~1~ y~",
        "x y y~ y.~1~ none: x y y.~1~ y~", "x y y~ y.~1~ off: x y y.~1~ y~", "x y y~ y.~1~ numbered: x y y.~1~ y.~2~ y~", "x y y~ y.~1~ t: x y y.~1~ y.~2~ y~", "x y y~ y.~1~ existing: x y y.~1~ y.~2~ y~", "x y y~ y.~1~ nil: x y y.~1~ y.~2~ y~", "x y y~ y.~1~ simple: x y y.~1~ y~", "x y y~ y.~1~ never: x y y.~1~ y~"
    };

    // First for cp
    for (int i = 0; i < 40; i++) {
        strcat(expected_buf, expected_lines[i]);
        strcat(expected_buf, "\n");
    }

    // Then for mv (where 'x' is removed, i.e. sed 's/: x/:/' )
    for (int i = 0; i < 40; i++) {
        char line[256];
        strcpy(line, expected_lines[i]);
        char *colon_x = strstr(line, ": x");
        if (colon_x) {
            memmove(colon_x + 1, colon_x + 3, strlen(colon_x + 3) + 1);
        }
        strcat(expected_buf, line);
        strcat(expected_buf, "\n");
    }

    // Run the actual loops
    for (int p = 0; p < 2; p++) {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 8; j++) {
                cleanup();
                
                // Touch initial files
                char init_copy[256];
                strcpy(init_copy, initial_files_cases[i]);
                char *tok = strtok(init_copy, " ");
                while (tok) {
                    create_file(tok);
                    tok = strtok(NULL, " ");
                }

                char cmd[256];
                snprintf(cmd, sizeof(cmd), "%s --backup=%s x y", progs[p], opts[j]);
                char out[1024];
                int res = exec_capture(cmd, out, sizeof(out));
                if (res != 0) {
                    printf("Failed command: %s, output: %s\n", cmd, out);
                }
                ASSERT_EQ(0, res);

                char *ls_out = ls_xy();
                
                char actual_line[512];
                snprintf(actual_line, sizeof(actual_line), "%s %s: %s\n", initial_files_cases[i], opts[j], ls_out);
                strcat(actual_buf, actual_line);
                
                free(ls_out);
            }
        }
    }

    cleanup();

    if (strcmp(expected_buf, actual_buf) != 0) {
        printf("Expected:\n%s\n", expected_buf);
        printf("Actual:\n%s\n", actual_buf);
        ASSERT_STR_EQ(expected_buf, actual_buf);
    }

    PASS();
}





SUITE(cp_mv_backup_suite) {
    RUN_TEST(test_cp_mv_backup);
}
DEFINE_TEST_MAIN(cp_mv_backup_suite)
