#pragma once

#include "Catmull-RomCurveEvaluator.h"
#include <assert.h>
#include "mat.h"
#include "vec.h"
#include "BezierCurveEvaluator.h"
#include <algorithm>
void CatmullRomCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();

	Mat4f Mb = Mat4f(-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 3, 0, 0,
		1, 0, 0, 0
	);

	double t = tension;
	Mat4f M_T = Mat4f(
		0, 1, 0, 0,
		-t / 3, 1, t / 3, 0,
		0, t / 3, 1, -t / 3,
		0, 0, 1, 0
	);
	Mat4f M_C = Mb * M_T;

	ptvEvaluatedCurvePts.clear();

	if (has_inner_control)
	{
		if (need_refind)
		{
			m_ptvInnerCtrlPts.clear();
			if (bWrap) {

				for (int i = 0; i < iCtrlPtCount; i++)
				{
					float delta = 0.0;
					if (ptvCtrlPts[(i + 1) % iCtrlPtCount].x + fAniLength * (int)((i + 1) / iCtrlPtCount) -
						ptvCtrlPts[(i + 2) % iCtrlPtCount].x - fAniLength * (int)((i + 2) / iCtrlPtCount) < 0.001)
						delta = 0.001;
					Vec4f M_x_To_t = Vec4f(ptvCtrlPts[(i + 0) % iCtrlPtCount].x + fAniLength * (int)((i + 0) / iCtrlPtCount),
						ptvCtrlPts[(i + 1) % iCtrlPtCount].x + fAniLength * (int)((i + 1) / iCtrlPtCount),
						ptvCtrlPts[(i + 2) % iCtrlPtCount].x + fAniLength * (int)((i + 2) / iCtrlPtCount),
						ptvCtrlPts[(i + 3) % iCtrlPtCount].x + fAniLength * (int)((i + 3) / iCtrlPtCount));
					Vec4f ctrlP_x = M_T * M_x_To_t;				
					M_x_To_t = M_C * M_x_To_t;

					Vec4f M_y_To_t = Vec4f(ptvCtrlPts[(i + 0) % iCtrlPtCount].y,
						ptvCtrlPts[(i + 1) % iCtrlPtCount].y,
						ptvCtrlPts[(i + 2) % iCtrlPtCount].y,
						ptvCtrlPts[(i + 3) % iCtrlPtCount].y);
					Vec4f ctrlP_y = M_T * M_y_To_t;
					M_y_To_t = M_C * M_y_To_t;

					m_ptvInnerCtrlPts.push_back(Point(ctrlP_x[1], ctrlP_y[1]));
					m_ptvInnerCtrlPts.push_back(Point(2 * ptvCtrlPts[i + 1].x - ctrlP_x[1], 2 * ptvCtrlPts[i + 1].y - ctrlP_y[1]));


					for (double t = 0; t < 1; t += s_fFlatnessEpsilon)
					{
						Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
						float x = quadric * M_x_To_t;
						if (x > ptvCtrlPts[(i + 2) % iCtrlPtCount].x + fAniLength * (int)((i + 2) / iCtrlPtCount))
							x = ptvCtrlPts[(i + 2) % iCtrlPtCount].x + fAniLength * (int)((i + 2) / iCtrlPtCount) - s_fFlatnessEpsilon * 10;
						else if (x < ptvCtrlPts[(i + 1) % iCtrlPtCount].x + fAniLength * (int)((i + 1) / iCtrlPtCount))
							x = ptvCtrlPts[(i + 1) % iCtrlPtCount].x + fAniLength * (int)((i + 1) / iCtrlPtCount) + s_fFlatnessEpsilon * 10;
						ptvEvaluatedCurvePts.push_back(Point(x - fAniLength * (int)(x > fAniLength), quadric * M_y_To_t));
					}
				}
			}
			else {

				for (int i = -1; i < iCtrlPtCount - 2; i++)
				{

					Vec4f M_x_To_t = Vec4f(ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 0, 0))].x,
						ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 1, 0))].x,
						ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 2, 0))].x,
						ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 3, 0))].x);
					Vec4f ctrlP_x = M_T * M_x_To_t;
					M_x_To_t = M_C * M_x_To_t;

					Vec4f M_y_To_t = Vec4f(ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 0, 0))].y,
						ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 1, 0))].y,
						ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 2, 0))].y,
						ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 3, 0))].y);
					Vec4f ctrlP_y = M_T * M_y_To_t;
					M_y_To_t = M_C * M_y_To_t;

					m_ptvInnerCtrlPts.push_back(Point(ctrlP_x[1], ctrlP_y[1]));
					m_ptvInnerCtrlPts.push_back(Point(2 * ptvCtrlPts[i + 1].x - ctrlP_x[1], 2 * ptvCtrlPts[i + 1].y - ctrlP_y[1]));


					if (i == iCtrlPtCount - 3)
					{
						m_ptvInnerCtrlPts.push_back(Point(ctrlP_x[2], ctrlP_y[2]));
						m_ptvInnerCtrlPts.push_back(Point(2 * ptvCtrlPts[i + 2].x - ctrlP_x[2], 2 * ptvCtrlPts[i + 2].y - ctrlP_y[2]));
					}

					for (double t = 0.0; t < 1; t += s_fFlatnessEpsilon)
					{
						Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
						float x = quadric * M_x_To_t;
						if (x > ptvCtrlPts[i + 2].x)
							x = ptvCtrlPts[i + 2].x - s_fFlatnessEpsilon * 10;
						else if (x < ptvCtrlPts[i + 1].x)
							x = ptvCtrlPts[i + 1].x + s_fFlatnessEpsilon * 10;
						ptvEvaluatedCurvePts.push_back(Point(x, quadric * M_y_To_t));
					}
				}

				ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
				ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[iCtrlPtCount - 1].y));
			}
			std::sort(m_ptvInnerCtrlPts.begin(),
				m_ptvInnerCtrlPts.end(),
				PointSmallerXCompare());
			need_refind = false;
		}
		else
		{
			std::vector<Point> new_CtrlPoints;
			new_CtrlPoints.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
			if (bWrap)
			{
				for (int i = 1; i < m_ptvInnerCtrlPts.size(); i++)
				{
					new_CtrlPoints.push_back(m_ptvInnerCtrlPts[i]);
				}
				new_CtrlPoints.push_back(Point(m_ptvInnerCtrlPts[0].x + fAniLength,
					m_ptvInnerCtrlPts[0].y));
			}
			else
			{
				for (int i = 1; i < m_ptvInnerCtrlPts.size()-1; i++)
				{
					new_CtrlPoints.push_back(m_ptvInnerCtrlPts[i]);
				}
			}
			std::sort(new_CtrlPoints.begin(),
				new_CtrlPoints.end(),
				PointSmallerXCompare());
			BezierCurveEvaluator c = BezierCurveEvaluator();
			c.evaluateCurve(new_CtrlPoints, ptvEvaluatedCurvePts, fAniLength, bWrap);
		}
	}
	else
	{
		if (bWrap) {

			for (int i = 0; i < iCtrlPtCount; i++)
			{
				float delta = 0.0;
				if (ptvCtrlPts[(i + 1) % iCtrlPtCount].x + fAniLength * (int)((i + 1) / iCtrlPtCount) -
					ptvCtrlPts[(i + 2) % iCtrlPtCount].x - fAniLength * (int)((i + 2) / iCtrlPtCount) < 0.001)
					delta = 0.001;
				Vec4f M_x_To_t = Vec4f(ptvCtrlPts[(i + 0) % iCtrlPtCount].x + fAniLength * (int)((i + 0) / iCtrlPtCount),
					ptvCtrlPts[(i + 1) % iCtrlPtCount].x + fAniLength * (int)((i + 1) / iCtrlPtCount),
					ptvCtrlPts[(i + 2) % iCtrlPtCount].x + fAniLength * (int)((i + 2) / iCtrlPtCount),
					ptvCtrlPts[(i + 3) % iCtrlPtCount].x + fAniLength * (int)((i + 3) / iCtrlPtCount));
				M_x_To_t = M_C * M_x_To_t;

				Vec4f M_y_To_t = Vec4f(ptvCtrlPts[(i + 0) % iCtrlPtCount].y,
					ptvCtrlPts[(i + 1) % iCtrlPtCount].y,
					ptvCtrlPts[(i + 2) % iCtrlPtCount].y,
					ptvCtrlPts[(i + 3) % iCtrlPtCount].y);
				M_y_To_t = M_C * M_y_To_t;



				for (double t = 0; t < 1; t += s_fFlatnessEpsilon)
				{
					Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
					float x = quadric * M_x_To_t;
					if (x > ptvCtrlPts[(i + 2) % iCtrlPtCount].x + fAniLength * (int)((i + 2) / iCtrlPtCount))
						x = ptvCtrlPts[(i + 2) % iCtrlPtCount].x + fAniLength * (int)((i + 2) / iCtrlPtCount) - s_fFlatnessEpsilon * 10;
					else if (x < ptvCtrlPts[(i + 1) % iCtrlPtCount].x + fAniLength * (int)((i + 1) / iCtrlPtCount))
						x = ptvCtrlPts[(i + 1) % iCtrlPtCount].x + fAniLength * (int)((i + 1) / iCtrlPtCount) + s_fFlatnessEpsilon * 10;
					ptvEvaluatedCurvePts.push_back(Point(x - fAniLength * (int)(x > fAniLength), quadric * M_y_To_t));
				}
			}
		}
		else {

			for (int i = -1; i < iCtrlPtCount - 2; i++)
			{

				//if (abs(ptvCtrlPts[i + 1].x - ptvCtrlPts[i + 2].x) < 0.01)
				//{
				//	ptvEvaluatedCurvePts.push_back(ptvCtrlPts[i + 1]);
				//	ptvEvaluatedCurvePts.push_back(ptvCtrlPts[i + 2]);
				//	continue;
				//}
				Vec4f M_x_To_t = Vec4f(ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 0, 0))].x,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 1, 0))].x,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 2, 0))].x,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 3, 0))].x);
				M_x_To_t = M_C * M_x_To_t;

				Vec4f M_y_To_t = Vec4f(ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 0, 0))].y,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 1, 0))].y,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 2, 0))].y,
					ptvCtrlPts[min(iCtrlPtCount - 1, max(i + 3, 0))].y);
				M_y_To_t = M_C * M_y_To_t;



				for (double t = 0.0; t < 1; t += s_fFlatnessEpsilon)
				{
					Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
					float x = quadric * M_x_To_t;
					if (x > ptvCtrlPts[i + 2].x)
						x = ptvCtrlPts[i + 2].x - s_fFlatnessEpsilon * 10;
					else if (x < ptvCtrlPts[i + 1].x)
						x = ptvCtrlPts[i + 1].x + s_fFlatnessEpsilon * 10;
					ptvEvaluatedCurvePts.push_back(Point(x, quadric * M_y_To_t));
				}
			}

			//ptvEvaluatedCurvePts.push_back(ptvCtrlPts[0]);
			//ptvEvaluatedCurvePts.push_back(ptvCtrlPts[iCtrlPtCount - 1]);
			ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
			ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[iCtrlPtCount - 1].y));
		}
		need_refind = true;
		current_inner_control_point = -1;
	}


}
