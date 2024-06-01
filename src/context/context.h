#ifndef XRE_CONTEXT_H
#define XRE_CONTEXT_H


#include "./state.h"
#include <stdlib.h>
#include <stdarg.h>


struct XREContext;


typedef void (*Component)(struct XREContext *, va_list);


struct XREContext * xre_context_alloc(char const *, Component);
struct XREContext * xre_context_root_alloc(void);
void xre_context_destroy(struct XREContext *);

void xre_context_state_reset_index(struct XREContext *);

struct XREContextState * xre_context_get_next_state(struct XREContext *);

struct XREContextState * xre_context_append_new_state(
    struct XREContext * context,
    void *,
    XREContextStateOnDestroy
);


#ifndef XRE_CONFIG_NOT_STRUCT_ALIAS
typedef struct XREContext XREContext;
#endif


#endif
