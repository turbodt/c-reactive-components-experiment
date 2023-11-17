#include "./component.h"
#include "./base.h"


struct IComponent {};
struct ComponentPrivate {
    struct IComponent base;
    char const * type;
    void(*renderer)(struct IContext *, void const *);
};
typedef struct ComponentPrivate ComponentPrivate;


#define TO_PRIV(component) ((ComponentPrivate *)(component))
#define TO_PUB(component) ((struct IComponent *)(component))


struct IComponent * component_alloc(
    char const * type,
    ComponentRenderer renderer
) {
    ComponentPrivate * component = XRE_ALLOC(ComponentPrivate, 1);

    component->renderer = renderer;
    component->type = type;

    return TO_PUB(component);
};


void component_destroy(struct IComponent * component) {
    ComponentPrivate * cmp = TO_PRIV(component);
    XRE_FREE(cmp);
};


inline char const * component_get_type(struct IComponent * component) {
    return TO_PRIV(component)->type;
};


void component_render(
    struct IComponent *component,
    struct IContext *context,
    void const *props
) {
    ComponentPrivate * cmp = TO_PRIV(component);
    cmp->renderer(context, props);
};
