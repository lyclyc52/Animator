#pragma once
#ifndef INCLUDED_POLUNOMIAL_CURVE_EVALUATOR_H
#define INCLUDED_POLUNOMIAL_CURVE_EVALUATOR_H


#include "CurveEvaluator.h"
#include <assert.h>
#include "mat.h"
#include "vec.h"

//using namespace std;

class PolynomialCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;


};



#endif