#ifndef XRE_COMPONENT_H
#define XRE_COMPONENT_H


struct IContext;


struct IComponent;


typedef void (*ComponentRenderer)(struct IContext *, void const *);


struct IComponent * component_alloc(char const *, ComponentRenderer);
void component_destroy(struct IComponent *);


char const * component_get_type(struct IComponent *);
void component_render(struct IComponent *, struct IContext *, void const *);

#endif
