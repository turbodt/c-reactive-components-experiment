#include <test-shared.h>
#include <xre.h>

static double g_value = 0.0;
static int g_changed = 0;

static void comp(XREContext *ctx, va_list args) {
    double set_value = va_arg(args, double);
    XREStateDouble * s = xre_use_double(ctx, 1.0);
    xre_state_set_double(s, set_value);
    g_value = xre_state_get_double(s);
    g_changed = xre_state_double_has_changed(s);
}

static int test_double_state(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use_root(root, comp, 9.25);
    ASSERT_FLOAT_EQ(g_value, 9.25);
    ASSERT_EQ(g_changed, 1);
    xre_use_root(root, comp, 9.25);
    ASSERT_EQ(g_changed, 0);
    xre_context_destroy(root);
    return 0;
}

static Test tests[] = {
    {.name="double state set/get/changed", .call=&test_double_state},
    {.name="", .call=NULL},
};
static TestSuite suite = {.name="xre state double suite", .tests=tests};
TestSuite const * test_suite_xre_state_double_get_001(void) { return &suite; }
