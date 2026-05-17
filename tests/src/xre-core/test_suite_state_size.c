#include <test-shared.h>
#include <xre.h>

static size_t g_value = 0;
static int g_changed = 0;

static void comp(XREContext *ctx, va_list args) {
    size_t set_value = va_arg(args, size_t);
    XREStateSize * s = xre_use_size(ctx, 4);
    xre_state_set_size(s, set_value);
    g_value = xre_state_get_size(s);
    g_changed = xre_state_size_has_changed(s);
}

static int test_size_state(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use_root(root, comp, (size_t)33);
    ASSERT_EQ(g_value, 33);
    ASSERT_EQ(g_changed, 1);
    xre_use_root(root, comp, (size_t)33);
    ASSERT_EQ(g_changed, 0);
    xre_context_destroy(root);
    return 0;
}

static Test tests[] = {
    {.name="size state set/get/changed", .call=&test_size_state},
    {.name="", .call=NULL},
};
static TestSuite suite = {.name="xre state size suite", .tests=tests};
TestSuite const * test_suite_xre_state_size_get_001(void) { return &suite; }
