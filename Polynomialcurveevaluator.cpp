#include "PolynomialCurveEvaluator.h"
#include "CurveEvaluator.h"
#include <assert.h>
#include "mat.h"
#include "vec.h"

void PolynomialCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{

	ptvEvaluatedCurvePts.clear();
	//cout << 1 << endl;
	int iCtrlPtCount = ptvCtrlPts.size();
	if (bWrap)
	{

	}
	else
	{
		float* coefficient = new float[iCtrlPtCount];
		for (int i = 0; i < iCtrlPtCount; i++)
		{
			coefficient[i] = 1;
			for (int j = 0; j < iCtrlPtCount; j++)
			{
				if (j == i)
					continue;
				else
				{
					coefficient[i] *= ptvCtrlPts[i].x - ptvCtrlPts[j].x;
				}
			}
			coefficient[i] = ptvCtrlPts[i].y / coefficient[i];
			//cout << coefficient[i] << " ";
		}

		for (float t = ptvCtrlPts[0].x; t < ptvCtrlPts[iCtrlPtCount - 1].x; t += s_fFlatnessEpsilon * fAniLength)
		{
			float result = 0.0;
			for (int i = 0; i < iCtrlPtCount; i++)
			{
				float z = 1;
				for (int j = 0; j < iCtrlPtCount; j++)
				{
					if (j == i)
						continue;
					else
					{
						z *= (t - ptvCtrlPts[j].x);
					}
				}
				result += z * coefficient[i];
			}
			ptvEvaluatedCurvePts.push_back(Point(t, result));
		}
		ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[iCtrlPtCount - 1].y));
		delete[]coefficient;
	}
}
//void PolynomialRegressionCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
//	std::vector<Point>& ptvEvaluatedCurvePts,
//	const float& fAniLength,
//	const bool& bWrap) const
//{
	//float learning_rate = 1.5;
	//int num_loop = 15;
//	ptvEvaluatedCurvePts.clear();
//	//cout << 1 << endl;
//	int iCtrlPtCount = ptvCtrlPts.size();
//	float* coefficient = new float[iCtrlPtCount];
//	for (int i = 0; i < iCtrlPtCount; i++)
//	{
//		coefficient[i] = 1.0f;
//	}
//
//	for (int a = 0; a < num_loop* iCtrlPtCount; a++)
//	{
//		for (int i = 0; i < iCtrlPtCount; i++)
//		{
//			float result = 0.0;
//			for (int j = 0; j < iCtrlPtCount; j++)
//			{
//				result += pow(ptvCtrlPts[i].x/ fAniLength, 1.0 * j) * coefficient[j];
//			}
//			float delta =  ptvCtrlPts[i].y-result;
//
//			for (int j = 0; j < iCtrlPtCount; j++)
//			{
//				coefficient[i] += learning_rate*2.0 * delta * pow(ptvCtrlPts[i].x / fAniLength, 1.0 * j) / sqrt(i + a * iCtrlPtCount + 1);
//			}
//		}
//
//		//for (int i = 0; i < 5; i++)
//		//{
//		//	cout << coefficient[i] << " ";
//		//}
//		//cout << endl;
//	}
//
//
//	
//	for (float t = 0; t < fAniLength; t += s_fFlatnessEpsilon* fAniLength)
//	{
//		float result = 0.0;
//		for (int j = 0; j < iCtrlPtCount; j++)
//		{
//			result += pow(t/20, 1.0 * j) * coefficient[j];
//		}
//		ptvEvaluatedCurvePts.push_back(Point(t, result));
//	}
//	delete[]coefficient;
//}