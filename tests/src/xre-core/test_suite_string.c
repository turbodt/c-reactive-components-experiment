#include <test-shared.h>
#include <xre.h>

static char const * g_text = NULL;
static size_t g_len = 0;
static int g_changed = 0;

static void comp_string(XREContext *ctx, va_list args) {
    char const * in = va_arg(args, char const *);
    XREStateString * s = xre_use_string(ctx, "hello");
    xre_state_set_string(s, in);
    g_text = xre_state_get_string(s);
    g_len = xre_state_get_string_len(s);
    g_changed = xre_state_string_has_changed(s);
}

static int test_string_roundtrip_and_flags(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use_root(root, comp_string, "abc");
    ASSERT_STR_EQ(g_text, "abc", "expected abc after second set");
    ASSERT_EQ(g_len, strlen("abc"), "expected len=3 after second set");
    ASSERT_NOT_ZERO(g_changed, "expected changed!=0 after second set");

    xre_use_root(root, comp_string, "abc");
    ASSERT_EQ(g_changed, 0, "expected changed=0 after equal set");

    xre_use_root(root, comp_string, NULL);
    ASSERT_STR_EQ(g_text, "", "expected empty after NULL set");
    ASSERT_EQ(g_len, 0, "expected len=0 after NULL set");
    ASSERT_NOT_ZERO(g_changed, "expected changed!=0 after NULL set");

    xre_use_root(root, comp_string, NULL);
    ASSERT_STR_EQ(g_text, "", "expected empty after second NULL set");
    ASSERT_EQ(g_len, 0, "expected len=0 after second NULL set");
    ASSERT_EQ(g_changed, 0, "expected changed=0 after second NULL set");

    xre_context_destroy(root);
    return 0;
}

static Test tests[] = {
    {.name="string get/set/len/changed", .call=&test_string_roundtrip_and_flags},
    {.name="", .call=NULL},
};

static TestSuite suite = {
    .name = "xre string suite",
    .tests = tests,
};

TestSuite const * test_suite_xre_string_get_001(void) { return &suite; }
