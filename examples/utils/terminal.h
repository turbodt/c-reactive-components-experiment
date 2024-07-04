#ifndef EXAMPLES_UTILS_TERMINAL_H
#define EXAMPLES_UTILS_TERMINAL_H


#include <stdlib.h>


typedef struct TerminalSize {
    size_t rows;
    size_t cols;
} TerminalSize;


TerminalSize get_terminal_size(void);


#endif
