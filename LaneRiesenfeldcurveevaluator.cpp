#pragma once
#include <assert.h>
#include "mat.h"
#include "vec.h"
#include "LaneRiesenfeldCurveEvaluator.h"
#include "B-splineCurveEvaluator.h"
#include <algorithm>
//using namespace std;



void LaneRiesenfeldCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();


	Mat4f M_B = Mat4f(-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 0, 3, 0,
		1, 4, 1, 0
	);
	if (bWrap) {
		if (iCtrlPtCount > 2)
		{
			std::vector<Point> new_Cpoints;

			for (int i = 0; i < iCtrlPtCount; i++)
			{
				new_Cpoints.push_back(ptvCtrlPts[i]);
				new_Cpoints.push_back(ptvCtrlPts[i]);
			}
			int degree = 4;
			int new_s = new_Cpoints.size();

			for (int i = 0; i < degree; i++)
			{
				std::vector<Point> last_Cpoints;
				last_Cpoints.assign(new_Cpoints.begin(), new_Cpoints.end());

				float x = 1.0 / 4.0 * (last_Cpoints[new_s - 1].x- fAniLength + 2 * last_Cpoints[0].x + last_Cpoints[1].x);
				float y = 1.0 / 4.0 * (last_Cpoints[new_s - 1].y + 2 * last_Cpoints[0].y + last_Cpoints[1].y);
				if (x < 0.0)
					x += fAniLength;
				new_Cpoints[0] = (Point(x, y));
				for (int j = 1; j < new_s-1; j++)
				{

					float x = 1.0 / 4.0 * (last_Cpoints[j - 1].x + 2 * last_Cpoints[j].x + last_Cpoints[j + 1].x);
					float y = 1.0 / 4.0 * (last_Cpoints[j - 1].y + 2 * last_Cpoints[j].y + last_Cpoints[j + 1].y);
					new_Cpoints[j] = (Point(x, y));
				}
				if (x > fAniLength)
					x -= fAniLength;
				x = 1.0 / 4.0 * (last_Cpoints[new_s - 2].x + 2 * last_Cpoints[new_s - 1].x + last_Cpoints[0].x+ fAniLength);
				y = 1.0 / 4.0 * (last_Cpoints[new_s - 2].y + 2 * last_Cpoints[new_s - 1].y + last_Cpoints[0].y+ fAniLength);
				
				new_Cpoints[0] = (Point(x, y));

				std::sort(new_Cpoints.begin(),
					new_Cpoints.end(),
					PointSmallerXCompare());

			}
			BsplineCurveEvaluator b = BsplineCurveEvaluator();
			b.evaluateCurve(new_Cpoints, ptvEvaluatedCurvePts, fAniLength, bWrap);
		}
		else
		{
			ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
			ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[iCtrlPtCount - 1].y));
			ptvEvaluatedCurvePts.push_back(ptvCtrlPts[0]);
			ptvEvaluatedCurvePts.push_back(ptvCtrlPts[iCtrlPtCount - 1]);
		}
	}
	else
	{
		if (iCtrlPtCount > 2)
		{
			std::vector<Point> new_Cpoints;
			for (int i = 0; i < iCtrlPtCount; i++)
			{
				new_Cpoints.push_back(ptvCtrlPts[i]);
				new_Cpoints.push_back(ptvCtrlPts[i]);
			}
			int degree = 4;
			int new_s = new_Cpoints.size();
			for (int i = 0; i < degree; i++)
			{
				std::vector<Point> last_Cpoints;
				last_Cpoints.assign(new_Cpoints.begin(), new_Cpoints.end());

				new_Cpoints[0]=(Point(0.5 * (last_Cpoints[0].x + last_Cpoints[1].x), 0.5 * (last_Cpoints[0].y + last_Cpoints[1].y)));
				for (int j = 1; j < new_s-1; j++)
				{

					float x = 1.0 / 4.0 * (last_Cpoints[j - 1].x + 2 * last_Cpoints[j].x + last_Cpoints[j + 1].x);
					float y = 1.0 / 4.0 * (last_Cpoints[j - 1].y + 2 * last_Cpoints[j].y + last_Cpoints[j + 1].y);
					new_Cpoints[j]=(Point(x, y));
				}
				new_Cpoints[new_s-1]=(Point(0.5 * (last_Cpoints[new_s - 2].x + last_Cpoints[new_s - 1].x),
											0.5 * (last_Cpoints[new_s - 2].y + last_Cpoints[new_s - 1].y)));
			
			}
			BsplineCurveEvaluator b = BsplineCurveEvaluator();
			b.evaluateCurve(new_Cpoints, ptvEvaluatedCurvePts, fAniLength, bWrap);
		}
		else
		{
			ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
			ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[iCtrlPtCount - 1].y));
			ptvEvaluatedCurvePts.push_back( ptvCtrlPts[0]);
			ptvEvaluatedCurvePts.push_back(ptvCtrlPts[iCtrlPtCount - 1]);
		}
	}
}


