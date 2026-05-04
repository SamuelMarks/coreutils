#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <config.h>
#include <sys/stat.h>
#include <unistd.h>

#include "greatest.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
extern pid_t waitpid(pid_t pid, int *wstatus, int options);
#include <string.h>
#include <stdio.h>

extern const char *bin_dir;


#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// A tiny shell to execute pipelines without /bin/sh


static int my_system_fd(const char *cmdline, int capture_fd) {
    char *cmd = strdup(cmdline);
    char *cmds[20];
    int num_cmds = 0;
    
    char *p = cmd;
    char *start = p;
    int in_quote1 = 0, in_quote2 = 0;
    while (*p) {
        if (*p == '\'') in_quote1 = !in_quote1;
        else if (*p == '"') in_quote2 = !in_quote2;
        else if (*p == '|' && !in_quote1 && !in_quote2) {
            *p = '\0';
            cmds[num_cmds++] = start;
            start = p + 1;
        }
        p++;
    }
    cmds[num_cmds++] = start;
    
    int prev_pipe = -1;
    pid_t pids[20];
    
    for (int i = 0; i < num_cmds; i++) {
        int pipefd[2];
        if (i < num_cmds - 1) {
            if (pipe(pipefd) < 0) {
                perror("pipe");
                exit(1);
            }
        }
        
        pids[i] = fork();
        if (pids[i] == 0) {
            if (prev_pipe != -1) {
                dup2(prev_pipe, STDIN_FILENO);
                close(prev_pipe);
            }
            if (i < num_cmds - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            } else if (capture_fd != -1) {
                dup2(capture_fd, STDOUT_FILENO);
            }
            
            char *args[100];
            int argc = 0;
            char *in_file = NULL, *out_file = NULL, *err_file = NULL;
            int append = 0;
            int err_to_out = 0;
            
            char *cp = cmds[i];
            while (*cp) {
                while (isspace((unsigned char)*cp)) cp++;
                if (!*cp) break;
                
                if (strncmp(cp, "2>&1", 4) == 0) {
                    err_to_out = 1; cp += 4;
                } else if (strncmp(cp, "2>", 2) == 0) {
                    cp += 2; while (isspace((unsigned char)*cp)) cp++;
                    char *f = cp; while (*cp && !isspace((unsigned char)*cp) && *cp != '>' && *cp != '<') cp++;
                    char tmp = *cp; if (*cp) { *cp = '\0'; cp++; }
                    err_file = f; if (tmp == '>' || tmp == '<') cp--;
                } else if (strncmp(cp, ">>", 2) == 0) {
                    append = 1; cp += 2; while (isspace((unsigned char)*cp)) cp++;
                    char *f = cp; while (*cp && !isspace((unsigned char)*cp) && *cp != '>' && *cp != '<') cp++;
                    char tmp = *cp; if (*cp) { *cp = '\0'; cp++; }
                    out_file = f; if (tmp == '>' || tmp == '<') cp--;
                } else if (*cp == '>') {
                    cp++; while (isspace((unsigned char)*cp)) cp++;
                    char *f = cp; while (*cp && !isspace((unsigned char)*cp) && *cp != '>' && *cp != '<') cp++;
                    char tmp = *cp; if (*cp) { *cp = '\0'; cp++; }
                    out_file = f; if (tmp == '>' || tmp == '<') cp--;
                } else if (*cp == '<') {
                    cp++; while (isspace((unsigned char)*cp)) cp++;
                    char *f = cp; while (*cp && !isspace((unsigned char)*cp) && *cp != '>' && *cp != '<') cp++;
                    char tmp = *cp; if (*cp) { *cp = '\0'; cp++; }
                    in_file = f; if (tmp == '>' || tmp == '<') cp--;
                } else {
                    char *arg = cp;
                    int q1 = 0, q2 = 0;
                    char *dest = cp;
                    while (*cp) {
                        if (*cp == '\'' && !q2) q1 = !q1;
                        else if (*cp == '"' && !q1) q2 = !q2;
                        else if ((isspace((unsigned char)*cp) || *cp == '>' || *cp == '<') && !q1 && !q2) break;
                        else *dest++ = *cp;
                        cp++;
                    }
                    char tmp = *cp; if (*cp) { *cp = '\0'; cp++; }
                    *dest = '\0';
                    args[argc++] = arg;
                    if (tmp == '>' || tmp == '<') cp--;
                }
            }
            args[argc] = NULL;
            
            if (in_file) {
                int fd = open(in_file, O_RDONLY);
                if (fd >= 0) { dup2(fd, STDIN_FILENO); close(fd); }
            }
            if (out_file) {
                int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
                int fd = open(out_file, flags, 0644);
                if (fd >= 0) { dup2(fd, STDOUT_FILENO); close(fd); }
            }
            if (err_file) {
                int fd = open(err_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd >= 0) { dup2(fd, STDERR_FILENO); close(fd); }
            }
            if (err_to_out) {
                dup2(STDOUT_FILENO, STDERR_FILENO);
            }
            
            if (argc > 0) {
                execvp(args[0], args);
            }
            exit(127);
        }
        
        if (prev_pipe != -1) close(prev_pipe);
        if (i < num_cmds - 1) {
            close(pipefd[1]);
            prev_pipe = pipefd[0];
        }
    }
    
    if (prev_pipe != -1) close(prev_pipe);
    if (capture_fd != -1) close(capture_fd);
    
    int status = 0;
    for (int i = 0; i < num_cmds; i++) {
        int s;
        waitpid(pids[i], &s, 0);
        if (i == num_cmds - 1) status = s;
    }
    free(cmd);
    return status;
}

static int my_system(const char *cmd) {
    return my_system_fd(cmd, -1);
}

static inline int exec_capture(const char *cmd, char *out, size_t out_size) {
    char temp_file[256];
    snprintf(temp_file, sizeof(temp_file), "/tmp/test_out_%d_%d.txt", getpid(), rand());
    
    int fd = open(temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    // We don't append redirections anymore. my_system_fd handles it.
    // However, exec_capture implies we capture stderr too if there is no 2>.
    // To keep it simple, let's just pass fd. If they want stderr, they do 2>&1.
    // Original exec_capture automatically did 2>&1 if "2>" wasn't present.
    char buf[16384];
    if (strstr(cmd, "2>") == NULL) {
        snprintf(buf, sizeof(buf), "%s 2>&1", cmd);
    } else {
        snprintf(buf, sizeof(buf), "%s", cmd);
    }
    
    int res = my_system_fd(buf, fd);
    
    FILE *f = fopen(temp_file, "rb");
    if (out && out_size > 0) out[0] = '\0';
    if (f) {
        if (out && out_size > 0) {
            size_t n = fread(out, 1, out_size - 1, f);
            out[n] = '\0';
        }
        fclose(f);
    }
    remove(temp_file);
    return res;
}



static inline void trim_newline(char *s) {
    char *p = strchr(s, '\r'); if (p) *p = '\0';
    p = strchr(s, '\n'); if (p) *p = '\0';
}

#ifdef _WIN32
#define PATH_SEP ";"
#else
#define PATH_SEP ":"
#endif

#define DEFINE_TEST_MAIN(SUITE_NAME) \
    const char *bin_dir = NULL; \
    GREATEST_MAIN_DEFS(); \
    int main(int argc, char **argv) { \
        char test_dir[256]; \
        snprintf(test_dir, sizeof(test_dir), "/tmp/ctest_%s_%d", #SUITE_NAME, getpid()); \
        mkdir(test_dir, 0755); \
        chown(test_dir, -1, getgid()); \
        chdir(test_dir); \
        if (argc < 2) { \
            fprintf(stderr, "Usage: %s <bin_dir>\n", argv[0]); \
            return 1; \
        } \
        bin_dir = argv[1]; \
        char new_path[4096]; \
        const char *old_path = getenv("PATH"); \
        snprintf(new_path, sizeof(new_path), "%s%s%s", bin_dir, PATH_SEP, old_path ? old_path : ""); \
        setenv("PATH", new_path, 1); \
        setenv("LC_ALL", "C", 1); \
        setenv("LANGUAGE", "C", 1); \
        int new_argc = argc - 1; \
        char **new_argv = malloc(new_argc * sizeof(char*)); \
        new_argv[0] = argv[0]; \
        for (int i = 2; i < argc; i++) new_argv[i - 1] = argv[i]; \
        GREATEST_MAIN_BEGIN(); \
        RUN_SUITE(SUITE_NAME); \
        GREATEST_MAIN_END(); \
        free(new_argv); \
        return 0; \
    }

#endif
