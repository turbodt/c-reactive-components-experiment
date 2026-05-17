#include <test-shared.h>
#include <xre.h>

static char g_value = 0;
static int g_changed = 0;

static void comp(XREContext *ctx, va_list args) {
    int set_value = va_arg(args, int);
    XREStateChar * s = xre_use_char(ctx, 'a');
    xre_state_set_char(s, (char)set_value);
    g_value = xre_state_get_char(s);
    g_changed = xre_state_char_has_changed(s);
}

static int test_char_state(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use_root(root, comp, 'z');
    ASSERT_EQ(g_value, 'z');
    ASSERT_EQ(g_changed, 1);
    xre_use_root(root, comp, 'z');
    ASSERT_EQ(g_changed, 0);
    xre_context_destroy(root);
    return 0;
}

static Test tests[] = {
    {.name="char state set/get/changed", .call=&test_char_state},
    {.name="", .call=NULL},
};
static TestSuite suite = {.name="xre state char suite", .tests=tests};
TestSuite const * test_suite_xre_state_char_get_001(void) { return &suite; }
