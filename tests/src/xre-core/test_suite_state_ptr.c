#include <test-shared.h>
#include <xre.h>

static void * g_value = NULL;
static int g_changed = 0;
static int g_a = 1;
static int g_b = 2;

static void comp(XREContext *ctx, va_list args) {
    void * set_value = va_arg(args, void *);
    XREStatePtr * s = xre_use_ptr(ctx, &g_a);
    xre_state_set_ptr(s, set_value);
    g_value = xre_state_get_ptr(s);
    g_changed = xre_state_ptr_has_changed(s);
}

static int test_ptr_state(void) {
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
    {.name="ptr state set/get/changed", .call=&test_ptr_state},
    {.name="", .call=NULL},
};
static TestSuite suite = {.name="xre state ptr suite", .tests=tests};
TestSuite const * test_suite_xre_state_ptr_get_001(void) { return &suite; }
