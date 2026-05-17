#include <test-shared.h>


static int test_001(void) {
    return 0;
}



static Test tests[] = {
    {.name="Example test", .call=&test_001},
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "Example test suite",
    .tests = tests,
};


TestSuite const * test_suite_module_example_get_001(void) {
    return &test_suite;
};
