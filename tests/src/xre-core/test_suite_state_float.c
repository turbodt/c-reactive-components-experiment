#include <test-shared.h>
#include <xre.h>

static float g_value = 0.0f;
static int g_changed = 0;

static void comp(XREContext *ctx, va_list args) {
    double set_value = va_arg(args, double);
    XREStateFloat * s = xre_use_float(ctx, 1.0f);
    xre_state_set_float(s, (float)set_value);
    g_value = xre_state_get_float(s);
    g_changed = xre_state_float_has_changed(s);
}

static int test_float_state(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use_root(root, comp, 3.5);
    ASSERT_FLOAT_EQ(g_value, 3.5f);
    ASSERT_EQ(g_changed, 1);
    xre_use_root(root, comp, 3.5);
    ASSERT_EQ(g_changed, 0);
    xre_context_destroy(root);
    return 0;
}

static Test tests[] = {
    {.name="float state set/get/changed", .call=&test_float_state},
    {.name="", .call=NULL},
};
static TestSuite suite = {.name="xre state float suite", .tests=tests};
TestSuite const * test_suite_xre_state_float_get_001(void) { return &suite; }
