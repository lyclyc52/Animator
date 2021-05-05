#include "CurveEvaluator.h"

float CurveEvaluator::s_fFlatnessEpsilon = 0.0001f;
int CurveEvaluator::s_iSegCount = 16;
float CurveEvaluator::tension = 0.5;
CurveEvaluator::~CurveEvaluator(void)
{
}
