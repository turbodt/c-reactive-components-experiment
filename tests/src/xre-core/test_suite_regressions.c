#include <test-shared.h>
#include <xre.h>
#include <signal.h>

static void comp_use_string_null(XREContext *ctx, va_list args) {
    (void) args;
    (void) xre_use_string(ctx, NULL);
}

static int case_use_string_null(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use_root(root, comp_use_string_null);
    xre_context_destroy(root);
    return 0;
}

static XREEffectCleanUp * effect_null_cleanup(va_list args) {
    (void) args;
    return xre_effect_clean_up_alloc(NULL, NULL);
}

static void comp_effect_null_cleanup(XREContext *ctx, va_list args) {
    int v = va_arg(args, int);
    XREStateInt * s = xre_use_int(ctx, 0);
    xre_state_set_int(s, v);
    XRERef const * deps[] = {&s->ref, NULL};
    xre_use_effect(ctx, effect_null_cleanup, deps);
}

static int case_effect_null_cleanup(void) {
    XREContext * root = xre_context_root_alloc();
    xre_use_root(root, comp_effect_null_cleanup, 1);
    xre_use_root(root, comp_effect_null_cleanup, 2);
    xre_context_destroy(root);
    return 0;
}

static int test_use_string_null_crashes_currently(void) {
    ASSERT_ISOLATED_EXIT_OK(case_use_string_null);
    return 0;
}

static int test_effect_null_cleanup_crashes_currently(void) {
    ASSERT_ISOLATED_SIGNALED(case_effect_null_cleanup, SIGSEGV);
    return 0;
}

static Test tests[] = {
    {.name="regression: xre_use_string(NULL) does not segfault", .call=&test_use_string_null_crashes_currently},
    {.name="regression: null cleanup fn segfaults", .call=&test_effect_null_cleanup_crashes_currently},
    {.name="", .call=NULL},
};

static TestSuite suite = {
    .name = "xre regression suite",
    .tests = tests,
};

TestSuite const * test_suite_xre_regressions_get_001(void) { return &suite; }
