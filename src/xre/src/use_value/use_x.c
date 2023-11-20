#include "./main.h"
#include "./use_x_factory.h"


XRE_USE_X_FACTORY(char, XREStateChar, int, char)
XRE_USE_X_FACTORY(int, XREStateInt, int, int)
XRE_USE_X_FACTORY(double, XREStateDouble, double, double)
XRE_USE_X_FACTORY(float, XREStateFloat, double, float)
