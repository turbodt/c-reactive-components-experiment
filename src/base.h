#ifndef XRE_BASE_H
#define XRE_BASE_H

#if defined(__cplusplus)
    #define CLITERAL(type) type
#else
    #define CLITERAL(type) (type)
#endif

#ifndef XRE_ALLOC
#include <stdlib.h>
#define XRE_ALLOC(type, n) ((type *) malloc(sizeof(type) * ((size_t) n)))
#endif

#ifndef XRE_REALLOC
#include <stdlib.h>
#define XRE_REALLOC(ptr, type, n) realloc(ptr, sizeof(type) * ((size_t)n))
#endif

#ifndef XRE_FREE
#include <stdlib.h>
#define XRE_FREE(ptr) if (!IS_NULL(ptr)) {\
    free((void *) (ptr));\
    ptr = NULL;\
}
#endif

#ifndef IS_NULL
#define IS_NULL(ptr) ((ptr) == NULL)
#endif

#ifndef XRE_ASSERT_ALLOC
#include <assert.h>
#define XRE_ASSERT_ALLOC(ptr) assert(!IS_NULL(ptr))
#endif

#ifndef XRE_ASSERT_ALLOC_OR
#define XRE_ASSERT_ALLOC_OR(ptr, alternative) if(IS_NULL(ptr)) {\
    alternative \
}
#endif

#ifndef RETURN_IF_NULL
#define RETURN_IF_NULL(ptr) if (IS_NULL(ptr)) {\
  return;\
}
#endif

#ifndef RETURN_V_IF_NULL
#define RETURN_V_IF_NULL(ptr, value) if (IS_NULL(ptr)) {\
  return value;\
}
#endif

#ifndef XRE_INDEX_NOT_FOUND
#include <limits.h>
#define XRE_INDEX_NOT_FOUND SIZE_MAX
#endif

#endif
