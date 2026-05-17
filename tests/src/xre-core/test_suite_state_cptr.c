#include <test-shared.h>
#include <xre.h>

static void const * g_value = NULL;
static int g_changed = 0;
static int const g_a = 1;
static int const g_b = 2;

static void comp(XREContext *ctx, va_list args) {
    void const * set_value = va_arg(args, void const *);
    XREStateCPtr * s = xre_use_cptr(ctx, &g_a);
    xre_state_set_cptr(s, set_value);
    g_value = xre_state_get_cptr(s);
    g_changed = xre_state_cptr_has_changed(s);
}

static int test_cptr_state(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use_root(root, comp, &g_b);
    ASSERT_EQ(g_value, &g_b);
    ASSERT_EQ(g_changed, 1);
    xre_use_root(root, comp, &g_b);
    ASSERT_EQ(g_changed, 0);
    xre_context_destroy(root);
    return 0;
}

static Test tests[] = {
    {.name="cptr state set/get/changed", .call=&test_cptr_state},
    {.name="", .call=NULL},
};
static TestSuite suite = {.name="xre state cptr suite", .tests=tests};
TestSuite const * test_suite_xre_state_cptr_get_001(void) { return &suite; }
