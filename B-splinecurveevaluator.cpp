#pragma once
#pragma once
#include "B-splineCurveEvaluator.h"
#include <assert.h>
#include "mat.h"
#include "vec.h"
void BsplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
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
	M_B = M_B / 6;

	ptvEvaluatedCurvePts.clear();

	if (bWrap) {

		for (int i = 0; i < iCtrlPtCount; i++)
		{
			Vec4f M_x_To_t = Vec4f(ptvCtrlPts[(i + 0)% iCtrlPtCount].x + fAniLength* (int)((i + 0) > iCtrlPtCount),
				ptvCtrlPts[(i + 1) % iCtrlPtCount ].x + fAniLength * (int)((i + 1) / iCtrlPtCount),
				ptvCtrlPts[(i + 2) % iCtrlPtCount ].x + fAniLength * (int)((i + 2) / iCtrlPtCount),
				ptvCtrlPts[(i + 3) % iCtrlPtCount ].x + fAniLength * (int)((i + 3) / iCtrlPtCount));
			M_x_To_t = M_B * M_x_To_t;

			Vec4f M_y_To_t = Vec4f(ptvCtrlPts[(i + 0) % iCtrlPtCount].y,
				ptvCtrlPts[(i + 1) % iCtrlPtCount].y,
				ptvCtrlPts[(i + 2) % iCtrlPtCount].y,
				ptvCtrlPts[(i + 3) % iCtrlPtCount].y);
			M_y_To_t = M_B * M_y_To_t;



			for (double t = 0; t < 1; t += s_fFlatnessEpsilon)
			{
				Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
				ptvEvaluatedCurvePts.push_back(Point(quadric * M_x_To_t - fAniLength * (int)(quadric * M_x_To_t > fAniLength), quadric * M_y_To_t));
			}
		}

	}
	else {


			for (int i = -3; i < iCtrlPtCount - 1; i++)
			{
				Vec4f M_x_To_t = Vec4f(ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 0, 0))].x,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 1, 0))].x,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 2, 0))].x,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 3, 0))].x);
				M_x_To_t = M_B * M_x_To_t;

				Vec4f M_y_To_t = Vec4f(ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 0, 0))].y,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 1, 0))].y,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 2, 0))].y,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 3, 0))].y);

				M_y_To_t = M_B * M_y_To_t;


				for (double t = 0; t < 1; t += s_fFlatnessEpsilon)
				{
					Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
					ptvEvaluatedCurvePts.push_back(Point(quadric * M_x_To_t, quadric * M_y_To_t));
				}
			}





		ptvEvaluatedCurvePts.push_back(Point(0.0,ptvCtrlPts[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[iCtrlPtCount - 1].y));
	}


}
