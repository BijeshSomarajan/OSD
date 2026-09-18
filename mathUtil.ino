#include "mathUtil.h"

//Handles bit shifting of -ve numbers
int32_t shiftBitsLeft32(int32_t value, int32_t nBits) {
	if (value < 0) {
		return (-((-value) << nBits));
	} else {
		return (value << nBits);
	}
}

//Converts Radians to Degrees
float convertRadToDeg(float rads) {
  return rads * OneEightyByPI;
}

//Converts Radians to Degrees
float convertDegToRad(float deg) {
  return deg * PIByOneEighty;
}

float sinPolyCoef3 = -1.666568107e-1f;  // cubic term for sin approximation
float sinPolyCoef5 = 8.312366210e-3f;   // 5th-order term
float sinPolyCoef7 = -1.849218155e-4f;  // 7th-order term

float sinApproxF(float x) {
	while (x > ONE_PI) {
		x -= TWO_PI;
	}
	while (x < -ONE_PI) {
		x += TWO_PI;
	}

	if (x > HALF_PI) {
		x = ONE_PI - x;
	} else if (x < -HALF_PI) {
		x = -ONE_PI - x;
	}

	float x2 = x * x;
	return x + x * x2 * (sinPolyCoef3 + x2 * (sinPolyCoef5 + x2 * sinPolyCoef7));
}

float cosApproxF(float x) {
	return sinApproxF(x + HALF_PI);
}

float tanApproxF(float x) {
	float c = cosApproxF(x);
	if (fabsf(c) < 1e-6f) {
		return 0.0f; // avoid division by near-zero
	}
	return sinApproxF(x) / c;
}