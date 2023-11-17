#ifndef XRE_COMPONENT_H
#define XRE_COMPONENT_H


struct IContext;


struct IComponentState {
    void * value;
};


struct IComponent {
    char const * type;
};


typedef void (*ComponentRenderer)(struct IContext *, void const *);


struct IComponent * component_alloc(char const *, ComponentRenderer);
void component_destroy(struct IComponent *);


void component_render(struct IComponent *, struct IContext *, void const *);


#endif
