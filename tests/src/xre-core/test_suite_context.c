#include <test-shared.h>
#include <xre.h>

static int g_counter = 0;
static int g_key_a = 0;
static int g_key_b = 0;

static void comp_count(XREContext *ctx, va_list args) {
    (void) ctx;
    (void) args;
    g_counter++;
}

static void comp_keyed(XREContext *ctx, va_list args) {
    int * dst = va_arg(args, int *);
    XREStateInt * state = xre_use_int(ctx, 0);
    int curr = xre_state_get_int(state) + 1;
    xre_state_set_int(state, curr);
    *dst = curr;
}

static int test_root_alloc_destroy(void) {
    XREContext * root = xre_context_root_alloc();
    ASSERT_NOT_NULL(root);
    xre_context_destroy(root);
    return 0;
}

static int test_use_root_calls_component(void) {
    XREContext * root = xre_context_root_alloc();
    g_counter = 0;
    xre_use_root(root, comp_count);
    xre_use_root(root, comp_count);
    ASSERT_EQ(g_counter, 2);
    xre_context_destroy(root);
    return 0;
}

static int test_child_context_reused_same_key(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use(root, "k", comp_keyed, &g_key_a);
    xre_use(root, "k", comp_keyed, &g_key_b);
    ASSERT_EQ(g_key_a, 1);
    ASSERT_EQ(g_key_b, 2);
    xre_context_destroy(root);
    return 0;
}

static int test_child_context_isolated_different_keys(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use(root, "a", comp_keyed, &g_key_a);
    xre_use(root, "b", comp_keyed, &g_key_b);
    ASSERT_EQ(g_key_a, 1);
    ASSERT_EQ(g_key_b, 1);
    xre_context_destroy(root);
    return 0;
}

static int test_use_ikey_reuses_key(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use_ikey(root, 42, comp_keyed, &g_key_a);
    xre_use_ikey(root, 42, comp_keyed, &g_key_b);
    ASSERT_EQ(g_key_a, 1);
    ASSERT_EQ(g_key_b, 2);
    xre_context_destroy(root);
    return 0;
}

static Test tests[] = {
    {.name="root alloc/destroy", .call=&test_root_alloc_destroy},
    {.name="use_root calls component", .call=&test_use_root_calls_component},
    {.name="child context reused by key", .call=&test_child_context_reused_same_key},
    {.name="child context isolated by key", .call=&test_child_context_isolated_different_keys},
    {.name="use_ikey reuses integer key", .call=&test_use_ikey_reuses_key},
    {.name="", .call=NULL},
};

static TestSuite suite = {
    .name = "xre context/use suite",
    .tests = tests,
};

TestSuite const * test_suite_xre_context_get_001(void) { return &suite; }
