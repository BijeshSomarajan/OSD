#ifndef UTILS_MATHUTILS_H__
#define UTILS_MATHUTILS_H__

#include <math.h>
#include <inttypes.h>

#define PI_Val 3.14159265358979323846f
#define ONE_PI  PI_Val
#define HALF_PI 1.57079632679489661923f
#define TWO_PI  6.28318530717958647692f

#define OneEightyByPI 180.0f / PI_Val
#define PIByOneEighty PI_Val / 180.0f

float sinApproxF(float x);
float cosApproxF(float x);
float tanApproxF(float x);

float convertDegToRad(float deg);
float convertRadToDeg(float rads);
int32_t shiftBitsLeft32(int32_t value, int32_t nBits);

#endif
