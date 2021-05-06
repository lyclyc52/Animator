#include "CurveEvaluator.h"

float CurveEvaluator::s_fFlatnessEpsilon = 0.0001f;
int CurveEvaluator::s_iSegCount = 16;
float CurveEvaluator::tension = 0.5;
bool CurveEvaluator::isMask = false;
std::vector<float> CurveEvaluator::mask;

bool CurveEvaluator::has_inner_control=false;
bool CurveEvaluator::need_refind = true;
std::vector<Point> CurveEvaluator::m_ptvInnerCtrlPts;
int CurveEvaluator::current_inner_control_point = -1;

CurveEvaluator::~CurveEvaluator(void)
{
}


int CurveEvaluator::getCurrentControlPoint()
{
	return current_inner_control_point;
}
bool CurveEvaluator::getHasInnerControl()
{
	return has_inner_control;
}
void CurveEvaluator::getInnerControlPoints(std::vector<Point>& p)
{
	p = m_ptvInnerCtrlPts;
}