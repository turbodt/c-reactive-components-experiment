#ifndef TESTS_XRE_CORE_H
#define TESTS_XRE_CORE_H

#include <test-shared.h>

TestSuite const * test_suite_xre_context_get_001(void);
TestSuite const * test_suite_xre_ref_get_001(void);
TestSuite const * test_suite_xre_effect_get_001(void);
TestSuite const * test_suite_xre_string_get_001(void);

TestSuite const * test_suite_xre_state_int_get_001(void);
TestSuite const * test_suite_xre_state_char_get_001(void);
TestSuite const * test_suite_xre_state_size_get_001(void);
TestSuite const * test_suite_xre_state_float_get_001(void);
TestSuite const * test_suite_xre_state_double_get_001(void);
TestSuite const * test_suite_xre_state_ptr_get_001(void);
TestSuite const * test_suite_xre_state_cptr_get_001(void);

TestSuite const * test_suite_xre_regressions_get_001(void);

#endif
