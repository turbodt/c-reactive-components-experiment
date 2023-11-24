#include "./terminal.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>


struct TerminalSize get_terminal_size(void) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    return (struct TerminalSize) {
        .rows=w.ws_row,
        .cols=w.ws_col
    };
};
