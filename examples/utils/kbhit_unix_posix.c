#include "./kbhit.h"
#include "./base.h"
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>


static struct termios * oldt = NULL;
static int * oldf = NULL;


void kb_init(void) {
    oldt = ALLOC(struct termios, 1);
    ASSERT_ALLOC(oldt);

    oldf = ALLOC(int, 1);
    ASSERT_ALLOC(oldf);

    struct termios newt;

    tcgetattr(STDIN_FILENO, oldt);
    newt = *oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    *oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, *oldf | O_NONBLOCK);
};


void kb_clean_up(void) {
    RETURN_IF_NULL(oldt);
    RETURN_IF_NULL(oldf);

    tcsetattr(STDIN_FILENO, TCSANOW, oldt);
    fcntl(STDIN_FILENO, F_SETFL, *oldf);

    FREE(oldt);
    FREE(oldf);
}


inline int kbhit(void) {
    ASSERT_ALLOC(oldt);
    ASSERT_ALLOC(oldf);
    return getchar();
}


