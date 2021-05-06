#pragma once
#include "BezierCurveEvaluator.h"
#include <assert.h>
#include "mat.h"
#include "vec.h"
void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();

	Mat4f Mb = Mat4f(-1,  3, -3,  1,
					  3, -6,  3,  0,
					 -3,  3,  0,  0,
					  1,  0,  0,  0
	);


	ptvEvaluatedCurvePts.clear();

	if (bWrap) {
		int spliteCurveCount = iCtrlPtCount / 3;
		int remainingCtrPoints = iCtrlPtCount % 3;

		for (int i = 0; i < spliteCurveCount; i++)
		{
			Vec4f M_x_To_t = Vec4f(ptvCtrlPts[i * 3 + 0].x,
				ptvCtrlPts[i * 3 + 1].x,
				ptvCtrlPts[i * 3 + 2].x,
				ptvCtrlPts[(i * 3 + 3) % iCtrlPtCount].x+fAniLength*(int)(i * 3 + 3>= iCtrlPtCount));
			M_x_To_t = Mb * M_x_To_t;

			Vec4f M_y_To_t = Vec4f(ptvCtrlPts[i * 3 + 0].y,
				ptvCtrlPts[i * 3 + 1].y,
				ptvCtrlPts[i * 3 + 2].y,
				ptvCtrlPts[(i * 3 + 3) % iCtrlPtCount].y);

			M_y_To_t = Mb * M_y_To_t;

			for (double t = 0; t < 1; t += s_fFlatnessEpsilon)
			{
				Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
				ptvEvaluatedCurvePts.push_back(Point(quadric * M_x_To_t- fAniLength * (int)(quadric * M_x_To_t>=fAniLength), quadric * M_y_To_t));
			}
		}

		if (remainingCtrPoints != 0)
		{
			float x = 0.0;
			float y1;


			if ((ptvCtrlPts[0].x + fAniLength) - ptvCtrlPts[iCtrlPtCount - 1].x > 0.0f) {
				y1 = (ptvCtrlPts[0].y * (fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x) +
					ptvCtrlPts[iCtrlPtCount - 1].y * ptvCtrlPts[0].x) /
					(ptvCtrlPts[0].x + fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x);
			}
			else
				y1 = ptvCtrlPts[0].y;


			ptvEvaluatedCurvePts.push_back(Point(x, y1));

			/// set the endpoint based on the wrap flag.
			float y2;
			x = fAniLength;
			y2 = y1;
			ptvEvaluatedCurvePts.push_back(Point(x, y2));
			
			ptvEvaluatedCurvePts.push_back(ptvCtrlPts[iCtrlPtCount - 1]);
		}
	}
	else {
		if (iCtrlPtCount < 4)
		{
			for (int i = 0; i < iCtrlPtCount; i++)
			{
				ptvEvaluatedCurvePts.push_back(ptvCtrlPts[i]);
			}
		}
		else
		{
			int spliteCurveCount = (iCtrlPtCount - 1) / 3;
			int remainingCtrPoints = (iCtrlPtCount - 1) % 3;
			for (int i = 0; i < spliteCurveCount; i++)
			{
				Vec4f M_x_To_t = Vec4f(ptvCtrlPts[i * 3 + 0].x,
					ptvCtrlPts[i * 3 + 1].x,
					ptvCtrlPts[i * 3 + 2].x,
					ptvCtrlPts[i * 3 + 3].x);
				M_x_To_t = Mb * M_x_To_t;

				Vec4f M_y_To_t = Vec4f(ptvCtrlPts[i * 3 + 0].y,
					ptvCtrlPts[i * 3 + 1].y,
					ptvCtrlPts[i * 3 + 2].y,
					ptvCtrlPts[i * 3 + 3].y);
				M_y_To_t = Mb * M_y_To_t;
				for (double t = 0; t < 1; t += s_fFlatnessEpsilon)
				{
					Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
					ptvEvaluatedCurvePts.push_back(Point(quadric * M_x_To_t, quadric * M_y_To_t));
				}
			}

			for (int i = 0; i < remainingCtrPoints; i++)
			{
				ptvEvaluatedCurvePts.push_back(ptvCtrlPts[iCtrlPtCount - i - 1]);
			}

			
		}
		ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[iCtrlPtCount - 1].y));
	}


}
