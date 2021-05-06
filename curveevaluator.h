#ifndef INCLUDED_CURVE_EVALUATOR_H
#define INCLUDED_CURVE_EVALUATOR_H

#pragma warning(disable : 4786)

#include "Curve.h"

//using namespace std;

class CurveEvaluator
{
public:
	static float tension;
	static std::vector<float> mask;
	static bool isMask;
	static bool has_inner_control;
	static std::vector<Point> m_ptvInnerCtrlPts;
	static bool need_refind;
	static int current_inner_control_point;

	virtual ~CurveEvaluator(void);
	virtual void evaluateCurve(const std::vector<Point>& control_points, 
							   std::vector<Point>& evaluated_curve_points, 
							   const float& animation_length, 
							   const bool& wrap_control_points) const = 0;
	
	int getCurrentControlPoint();
	bool getHasInnerControl();
	void getInnerControlPoints(std::vector<Point>&);
	
	static float s_fFlatnessEpsilon;
	static int s_iSegCount;
};


#endif