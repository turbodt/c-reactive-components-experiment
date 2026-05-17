#include <test-shared.h>
#include <xre.h>

static int g_effect_calls = 0;
static int g_cleanup_calls = 0;

static void cleanup_fn(void * props) {
    (void) props;
    g_cleanup_calls++;
}

static XREEffectCleanUp * effect_fn(va_list args) {
    (void) args;
    g_effect_calls++;
    return xre_effect_clean_up_alloc(cleanup_fn, NULL);
}

static void comp_effect(XREContext *ctx, va_list args) {
    int value = va_arg(args, int);
    XREStateInt * s = xre_use_int(ctx, 0);
    xre_state_set_int(s, value);
    XRERef const * deps[] = {&s->ref, NULL};
    xre_use_effect(ctx, effect_fn, deps);
}

static int test_effect_dependency_and_cleanup(void) {
    XREContext * root = xre_context_root_alloc();
    g_effect_calls = 0;
    g_cleanup_calls = 0;

    xre_use_root(root, comp_effect, 1);
    xre_use_root(root, comp_effect, 1);
    xre_use_root(root, comp_effect, 2);

    ASSERT_EQ(g_effect_calls, 2);
    ASSERT_EQ(g_cleanup_calls, 1);

    xre_context_destroy(root);
    ASSERT_EQ(g_cleanup_calls, 2);
    return 0;
}

static Test tests[] = {
    {.name="effect runs on deps change and cleanup", .call=&test_effect_dependency_and_cleanup},
    {.name="", .call=NULL},
};

static TestSuite suite = {
    .name = "xre effect suite",
    .tests = tests,
};

TestSuite const * test_suite_xre_effect_get_001(void) { return &suite; }
