#include "./kbhit.h"
#include "./base.h"
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>


static struct termios * oldt = NULL;
static int * oldf = NULL;


void kb_init(void) {
    oldt = XRE_ALLOC(struct termios, 1);
    XRE_ASSERT_ALLOC(oldt);

    oldf = XRE_ALLOC(int, 1);
    XRE_ASSERT_ALLOC(oldf);

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

    XRE_FREE(oldt);
    XRE_FREE(oldf);
}


inline char kbhit(void) {
    XRE_ASSERT_ALLOC(oldt);
    XRE_ASSERT_ALLOC(oldf);
    return getchar();
}


