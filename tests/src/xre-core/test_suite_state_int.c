#include <test-shared.h>
#include <xre.h>

static int g_value = 0;
static int g_changed = 0;

static void comp(XREContext *ctx, va_list args) {
    int set_value = va_arg(args, int);
    XREStateInt * s = xre_use_int(ctx, 7);
    xre_state_set_int(s, set_value);
    g_value = xre_state_get_int(s);
    g_changed = xre_state_int_has_changed(s);
}

static int test_int_state(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use_root(root, comp, 9);
    ASSERT_EQ(g_value, 9);
    ASSERT_EQ(g_changed, 1);
    xre_use_root(root, comp, 9);
    ASSERT_EQ(g_changed, 0);
    xre_context_destroy(root);
    return 0;
}

static Test tests[] = {
    {.name="int state set/get/changed", .call=&test_int_state},
    {.name="", .call=NULL},
};
static TestSuite suite = {.name="xre state int suite", .tests=tests};
TestSuite const * test_suite_xre_state_int_get_001(void) { return &suite; }
