#pragma once

#ifndef INCLUDED_LANRIESENFELD_CURVE_EVALUATOR_H
#define INCLUDED_LANRIESENFELD_CURVE_EVALUATOR_H


#include "CurveEvaluator.h"


//using namespace std;

class LaneRiesenfeldCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;


};



#endif