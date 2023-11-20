#ifndef XRE_USE_VALUE_MAIN_H
#define XRE_USE_VALUE_MAIN_H


#include "./use_x_factory.h"
#include "../context.h"


struct XREState;


XRE_USE_X_FACTORY_H(char, XREStateChar, char)
XRE_USE_X_FACTORY_H(int, XREStateInt, int)
XRE_USE_X_FACTORY_H(double, XREStateDouble, double)
XRE_USE_X_FACTORY_H(float, XREStateFloat, float)

#endif
