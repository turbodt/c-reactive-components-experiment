#ifndef TESTS_SHARED_MAIN_H
#define TESTS_SHARED_MAIN_H


#include "./test-shared/config.h"
#include "./test-shared/assertions_basic.h"
#include <sys/types.h>


#define CHANGE_ON_IMPL_MSG "This test is a placeholder until the feature" \
    " is implemented.\nThis assertion failing means that the feature has been"\
    " implemented and therefore this test should be updated."


#ifndef ALLOC
#include <stdlib.h>
#define ALLOC(type, n) (type *) malloc(sizeof(type) * ((size_t) n))
#endif
#ifndef REALLOC
#include <stdlib.h>
#define REALLOC(ptr, type, n) (type *) realloc(ptr, sizeof(type) * ((size_t) n))
#endif
#ifndef FREE
#include <stdlib.h>
#define FREE(ptr) if ( (ptr) != NULL) free(ptr)
#endif


#include <stdio.h>
#include <string.h>
#define LOG(first, ...) do {\
        printf("\x1b[30m");\
        printf(first, ## __VA_ARGS__ );\
        printf("\x1b[m");\
    } while(0)


#define LOG_SEP do {\
        printf("\n-----\n");\
    } while(0)


int test_run_isolated(TestFn fn, int * exit_code, int * term_sig);


#endif
