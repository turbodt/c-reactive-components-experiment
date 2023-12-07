#ifndef EXAMPLES_BASE_H
#define EXAMPLES_BASE_H

#ifndef ALLOC
#include <stdlib.h>
#define ALLOC(type, n) ((type *) malloc(sizeof(type) * ((size_t) n)))
#endif

#ifndef REALLOC
#include <stdlib.h>
#define REALLOC(ptr, type, n) realloc(ptr, sizeof(type) * ((size_t)n))
#endif

#ifndef FREE
#include <stdlib.h>
#define FREE(ptr) if (!IS_NULL(ptr)) {\
    free((void *) (ptr));\
    ptr = NULL;\
}
#endif

#ifndef IS_NULL
#define IS_NULL(ptr) ((ptr) == NULL)
#endif

#ifndef ASSERT_ALLOC
#include <assert.h>
#define ASSERT_ALLOC(ptr) assert(!IS_NULL(ptr))
#endif

#ifndef RETURN_IF_NULL
#define RETURN_IF_NULL(ptr) if (IS_NULL(ptr)) {\
  return;\
}
#endif

#endif
