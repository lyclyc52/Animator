#include "CurveEvaluator.h"

float CurveEvaluator::s_fFlatnessEpsilon = 0.0001f;
int CurveEvaluator::s_iSegCount = 16;
float CurveEvaluator::tension = 0.5;
bool CurveEvaluator::isMask = false;
std::vector<float> CurveEvaluator::mask;
bool CurveEvaluator::has_inner_control=false;
CurveEvaluator::~CurveEvaluator(void)
{
}
