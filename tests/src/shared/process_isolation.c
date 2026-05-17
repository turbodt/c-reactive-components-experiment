#include "./include/test-shared.h"
#include <unistd.h>
#include <sys/wait.h>


int test_run_isolated(TestFn fn, int * exit_code, int * term_sig) {
    if (fn == NULL) {
        return 1;
    }

    if (exit_code != NULL) {
        *exit_code = -1;
    }
    if (term_sig != NULL) {
        *term_sig = 0;
    }

    pid_t pid = fork();
    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        int err = fn();
        _exit(err);
    }

    int status = 0;
    pid_t wait_result = waitpid(pid, &status, 0);
    if (wait_result < 0) {
        return 1;
    }

    if (WIFEXITED(status)) {
        if (exit_code != NULL) {
            *exit_code = WEXITSTATUS(status);
        }
        return 0;
    }

    if (WIFSIGNALED(status)) {
        if (term_sig != NULL) {
            *term_sig = WTERMSIG(status);
        }
        return 0;
    }

    return 1;
}
