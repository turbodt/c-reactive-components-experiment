#ifndef TERMINAL_H
#define TERMINAL_H


#include <stdlib.h>


struct TerminalSize {
    size_t rows;
    size_t cols;
};


struct TerminalSize get_terminal_size(void);


#endif
