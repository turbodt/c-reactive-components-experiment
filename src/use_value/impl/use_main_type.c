#include "../use_main_type.h"
#include "./use_x_factory.h"


XRE_USE_X_FACTORY_IMPL(void *, XREStatePtr, void *, ptr);
XRE_USE_X_FACTORY_IMPL(void const *, XREStateCPtr, void const *, cptr);
XRE_USE_X_FACTORY_IMPL(char, XREStateChar, int, char);
XRE_USE_X_FACTORY_IMPL(int, XREStateInt, int, int);
XRE_USE_X_FACTORY_IMPL(size_t, XREStateSize, size_t, size);
XRE_USE_X_FACTORY_IMPL(double, XREStateDouble, double, double);
XRE_USE_X_FACTORY_IMPL(float, XREStateFloat, double, float);
