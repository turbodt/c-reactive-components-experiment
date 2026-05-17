#include <test-shared.h>
#include <xre.h>

static int g_ctor = 0;
static int g_dtor = 0;
static int g_assign = 0;
static int g_cmp = 0;

static void * ctor(va_list args) {
    int init = va_arg(args, int);
    int * p = ALLOC(int, 1);
    *p = init;
    g_ctor++;
    return p;
}

static void dtor(void * ptr) {
    g_dtor++;
    FREE(ptr);
}

static void assign(void * dst, void const * src) {
    g_assign++;
    *(int *)dst = *(int const *)src;
}

static int cmp(void const * a, void const * b) {
    g_cmp++;
    return *(int const *)a == *(int const *)b ? 0 : 1;
}

static void comp_ref(XREContext *ctx, va_list args) {
    (void) args;
    XRERef * ref = xre_use_ref_ex(ctx, ctor, dtor, assign, cmp, 3);
    int next = *(int *)xre_ref_get(ref) + 1;
    xre_ref_set(ref, &next);
}

static int test_ref_lifecycle_and_ops(void) {
    XREContext * root = xre_context_root_alloc();
    g_ctor = g_dtor = g_assign = g_cmp = 0;
    xre_use_root(root, comp_ref);
    xre_use_root(root, comp_ref);
    ASSERT_EQ(g_ctor, 1);
    ASSERT_TRUE(g_assign >= 2);
    ASSERT_TRUE(g_cmp >= 2);
    xre_context_destroy(root);
    ASSERT_EQ(g_dtor, 1);
    return 0;
}

static Test tests[] = {
    {.name="ref ctor/assign/cmp/dtor", .call=&test_ref_lifecycle_and_ops},
    {.name="", .call=NULL},
};

static TestSuite suite = {
    .name = "xre ref suite",
    .tests = tests,
};

TestSuite const * test_suite_xre_ref_get_001(void) { return &suite; }
