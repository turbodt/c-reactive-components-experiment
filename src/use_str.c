#include "./use_str.h"
#include "./xre/impl.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


static void * constructor(va_list);
static void destructor(void *);
static void assignator(void *, void const *);
static int comparator(void const *, void const *);


XRE_USE_X_FACTORY_IMPL_EX(
    char const *,
    StateStr,
    char const *,
    str,
    constructor,
    destructor,
    assignator,
    comparator
);


void * constructor(va_list args) {
    char ** value = (char **) malloc(sizeof(char *));
    *value = NULL;

    char const * new_value = (char const *) va_arg(args, char const *);
    assignator(value, &new_value);

    return value;
};


void destructor(void *ptr) {
    char * value = *(char **) ptr;
    free(ptr);
    free(value);
};


void assignator(void *dst, void const *src) {
    char ** p_dst = (char **) dst;
    char const * const * p_src = (char const * const*) src;

    size_t len_dst = IS_NULL(*p_dst) ? 0 : strlen(*p_dst);
    size_t len_src = strlen(*p_src);

    if (IS_NULL(*p_dst) || len_dst < len_src || len_dst > 2*len_src) {
        *p_dst = realloc(*p_dst, sizeof(char) * (len_src + 1));
    }

    strncpy(*p_dst, *p_src, len_src + 1);
};


int comparator(void const *a, void const *b) {
    char const * const * p_a = (char const * const *) a;
    char const * const * p_b = (char const * const *) b;
    return strcmp(*p_a, *p_b);
};
