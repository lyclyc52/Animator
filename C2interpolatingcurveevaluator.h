#ifndef INCLUDED_C2INTERPOLATING_CURVE_EVALUATOR_H
#define INCLUDED_C2INTERPOLATING_CURVE_EVALUATOR_H
#include <assert.h>
#include "mat.h"
#include "vec.h"
#include "CurveEvaluator.h"

//using namespace std;

class C2InterpolatingCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;
};

void C2InterpolatingCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
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


	Mat4f Ma= Mat4f(2,-2,1,1,
		-3,3,-2,-1,
		0,0,1,0,
		1, 0, 0, 0
	);

	//Mat4f(-1, 3, -3, 1,
	//	2, -5, 4, -1,
	//	-1, 0, 1, 0,
	//	0, 2, 0, 0
	//);

//M_C = M_C / 2;
	ptvEvaluatedCurvePts.clear();

	if (bWrap) {
		if (iCtrlPtCount>2)
		{
			//cout << fAniLength << endl;
			//for (int i = 0; i < iCtrlPtCount; i++)
			//{
			//	cout << ptvCtrlPts[i].x << " ";
			//}

			//cout << endl;
			float* diagonal = new float[iCtrlPtCount];
			float* x = new float[iCtrlPtCount];
			float* y = new float[iCtrlPtCount];
			float* rightmost = new float[iCtrlPtCount];
			diagonal[0] = 1.0 / 4.0;
			x[0] = 3.0 * (ptvCtrlPts[1].x - ptvCtrlPts[iCtrlPtCount - 1].x+ fAniLength) * diagonal[0];
			y[0] = 3.0 * (ptvCtrlPts[1].y - ptvCtrlPts[iCtrlPtCount - 1].y) * diagonal[0];
			rightmost[0] = 1.0 / 4.0;
			for (int i = 1; i < iCtrlPtCount - 2; i++)
			{
				diagonal[i] = 1.0 / (4.0 - diagonal[i - 1]);
				x[i] = (3.0 * (ptvCtrlPts[i + 1].x - ptvCtrlPts[i - 1].x) - x[i - 1]) * diagonal[i];
				y[i] = (3.0 * (ptvCtrlPts[i + 1].y - ptvCtrlPts[i - 1].y) - y[i - 1]) * diagonal[i];
				rightmost[i] = -rightmost[i - 1] * diagonal[i];
			}
			//cout << diagonal[0] << endl;
			diagonal[iCtrlPtCount - 2] = (1.0 - rightmost[iCtrlPtCount - 3]) / (4.0 - diagonal[iCtrlPtCount - 3]);
			x[iCtrlPtCount - 2] = (3.0 * (ptvCtrlPts[iCtrlPtCount - 1].x - ptvCtrlPts[iCtrlPtCount - 3].x) -
				x[iCtrlPtCount - 3]) / (4.0 - diagonal[iCtrlPtCount - 3]);
			y[iCtrlPtCount - 2] = (3.0 * (ptvCtrlPts[iCtrlPtCount - 1].y - ptvCtrlPts[iCtrlPtCount - 3].y) -
				y[iCtrlPtCount - 3]) / (4.0 - diagonal[iCtrlPtCount - 3]);


			diagonal[iCtrlPtCount - 1] = (4.0 - diagonal[iCtrlPtCount - 2]);
			x[iCtrlPtCount - 1] = (3.0 * (ptvCtrlPts[0].x - ptvCtrlPts[iCtrlPtCount - 2].x+ fAniLength) -
				x[iCtrlPtCount - 2]);
			y[iCtrlPtCount - 1] = (3.0 * (ptvCtrlPts[0].y - ptvCtrlPts[iCtrlPtCount - 2].y) -
				y[iCtrlPtCount - 2]);
			//cout << diagonal[0] << " " << diagonal[1] << " " << diagonal[2]<<endl;
			//cout << x[0] << " " << x[1] << " " << x[2] << endl;
			//float* ext = new float[iCtrlPtCount];
			//ext[0] = diagonal[iCtrlPtCount - 1];
			//cout << ext << endl;
			for (int i = 0; i < iCtrlPtCount - 2; i++)
			{
				diagonal[iCtrlPtCount - 1] = (diagonal[iCtrlPtCount - 1] -rightmost[i])/(-diagonal[i]);
				x[iCtrlPtCount - 1] -= x[i];
				x[iCtrlPtCount - 1] /= (-diagonal[i]);
				y[iCtrlPtCount - 1] -= y[i];
				y[iCtrlPtCount - 1] /= (-diagonal[i]);
				//cout << rightmost[i] << endl;
				//cout << ext << endl;
			}

			diagonal[iCtrlPtCount - 1] -= diagonal[iCtrlPtCount - 2] ;
			x[iCtrlPtCount - 1] -= x[iCtrlPtCount - 2] ;
			//x[iCtrlPtCount - 1] /= (1 - diagonal[iCtrlPtCount - 2] * ext);
			y[iCtrlPtCount - 1] -= y[iCtrlPtCount - 2] ;
			//y[iCtrlPtCount - 1] /= (1 - diagonal[iCtrlPtCount - 2] * ext);
			//cout << diagonal[iCtrlPtCount - 1] << endl;
			//cout << x[iCtrlPtCount - 1] << endl;
			float* der_x = new float[iCtrlPtCount];
			float* der_y = new float[iCtrlPtCount];
			der_x[iCtrlPtCount - 1] = x[iCtrlPtCount - 1]/ diagonal[iCtrlPtCount - 1];
			der_y[iCtrlPtCount - 1] = y[iCtrlPtCount - 1]/ diagonal[iCtrlPtCount - 1];
			der_x[iCtrlPtCount - 2] = x[iCtrlPtCount - 2] - der_x[iCtrlPtCount - 1]
				* diagonal[iCtrlPtCount - 2];
			der_y[iCtrlPtCount - 2] = y[iCtrlPtCount - 2] - der_y[iCtrlPtCount - 1]
				* diagonal[iCtrlPtCount - 2];
			for (int i = 2; i < iCtrlPtCount; i++)
			{
				der_x[iCtrlPtCount - 1 - i] = x[iCtrlPtCount - 1 - i] - der_x[iCtrlPtCount - i]
					* diagonal[iCtrlPtCount - 1 - i]-rightmost[iCtrlPtCount -1- i] * der_y[iCtrlPtCount - 1];
				der_y[iCtrlPtCount - 1 - i] = y[iCtrlPtCount - 1 - i] - der_y[iCtrlPtCount - i]
					* diagonal[iCtrlPtCount - 1 - i] - rightmost[iCtrlPtCount - 1 - i] * der_y[iCtrlPtCount - 1];
			}


			//for (int i = 0; i < iCtrlPtCount; i++)
			//{
			//	cout << der_x[i]<<" ";
			//}
			//cout << endl;
			for (int i = 0; i < iCtrlPtCount - 1; i++)
			{
				Vec4f M_x_To_t = Vec4f(ptvCtrlPts[i + 0].x,
					ptvCtrlPts[i + 0].x + der_x[i] / 3,
					ptvCtrlPts[i + 1].x - der_x[i + 1] / 3,
					ptvCtrlPts[i + 1].x);
				M_x_To_t = Mb * M_x_To_t;

				Vec4f M_y_To_t = Vec4f(ptvCtrlPts[i + 0].y,
					ptvCtrlPts[i + 0].y + der_y[i] / 3,
					ptvCtrlPts[i + 1].y - der_y[i + 1] / 3,
					ptvCtrlPts[i + 1].y);
				M_y_To_t = Mb * M_y_To_t;


				//cout << M_x_To_t[0] << " " << M_x_To_t[1] << " " << M_x_To_t[2] << " " << M_x_To_t[3] << endl;
				//cout << M_y_To_t[0] << " " << M_y_To_t[1] << " " << M_y_To_t[2] << " " << M_y_To_t[3] << endl;

				for (double t = 0.0; t <= 1; t += s_fFlatnessEpsilon)
				{
					Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
					float x = quadric * M_x_To_t;
					if (x > ptvCtrlPts[i + 1].x)
						x = ptvCtrlPts[i + 1].x - s_fFlatnessEpsilon;
					else if (x < ptvCtrlPts[i].x)
						x = ptvCtrlPts[i].x + s_fFlatnessEpsilon;
					ptvEvaluatedCurvePts.push_back(Point(x, quadric * M_y_To_t));
				}
				//cout << endl;
			}
			Vec4f M_x_To_t = Vec4f(ptvCtrlPts[iCtrlPtCount-1].x,
				ptvCtrlPts[iCtrlPtCount - 1].x + der_x[iCtrlPtCount - 1] / 3,
				ptvCtrlPts[0].x - der_x[0] / 3 + fAniLength,
				ptvCtrlPts[0].x + fAniLength);
			M_x_To_t = Mb * M_x_To_t;

			Vec4f M_y_To_t = Vec4f(ptvCtrlPts[iCtrlPtCount - 1].y,
				ptvCtrlPts[iCtrlPtCount - 1].y + der_y[iCtrlPtCount - 1] / 3,
				ptvCtrlPts[0].y - der_y[0] / 3,
				ptvCtrlPts[0].y);
			M_y_To_t = Mb * M_y_To_t;



			for (double t = 0.0; t <= 1; t += s_fFlatnessEpsilon)
			{
				Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
				float x = quadric * M_x_To_t;
				if (x > ptvCtrlPts[0].x + fAniLength)
					x = ptvCtrlPts[0].x + fAniLength - s_fFlatnessEpsilon;
				else if (x < ptvCtrlPts[iCtrlPtCount-1].x)
					x = ptvCtrlPts[iCtrlPtCount-1].x + s_fFlatnessEpsilon;
				ptvEvaluatedCurvePts.push_back(Point(x- fAniLength*(int)(x> fAniLength), quadric * M_y_To_t));
			}
		}
		else
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
		}
	}
	else {
		float* diagonal = new float[iCtrlPtCount];
		float* x = new float[iCtrlPtCount];
		float* y = new float[iCtrlPtCount];
		diagonal[0] = 1 / 2;
		x[0] = 3 * (ptvCtrlPts[1].x - ptvCtrlPts[0].x) * diagonal[0];
		y[0] = 3 * (ptvCtrlPts[1].y - ptvCtrlPts[0].y) * diagonal[0];

		for (int i = 1; i < iCtrlPtCount - 1; i++)
		{
			diagonal[i] = 1 / (4 - diagonal[i - 1]);
			x[i] = (3 * (ptvCtrlPts[i + 1].x - ptvCtrlPts[i - 1].x) - x[i - 1]) * diagonal[i];
			y[i] = (3 * (ptvCtrlPts[i + 1].y - ptvCtrlPts[i - 1].y) - y[i - 1]) * diagonal[i];
			//cout << diagonal[i] << endl;
		}
		diagonal[iCtrlPtCount - 1] = 1 / (2 - diagonal[iCtrlPtCount - 2]);
		x[iCtrlPtCount - 1] = (3 * (ptvCtrlPts[iCtrlPtCount - 1].x - ptvCtrlPts[iCtrlPtCount - 2].x) -
			x[iCtrlPtCount - 2]) * diagonal[iCtrlPtCount - 2];
		y[iCtrlPtCount - 1] = (3 * (ptvCtrlPts[iCtrlPtCount - 1].y - ptvCtrlPts[iCtrlPtCount - 2].y) -
			y[iCtrlPtCount - 2]) * diagonal[iCtrlPtCount - 2];

		float* der_x = new float[iCtrlPtCount];
		float* der_y = new float[iCtrlPtCount];
		der_x[iCtrlPtCount - 1] = x[iCtrlPtCount - 1];
		der_y[iCtrlPtCount - 1] = y[iCtrlPtCount - 1];
		for (int i = 1; i < iCtrlPtCount; i++)
		{
			der_x[iCtrlPtCount - 1 - i] = x[iCtrlPtCount - 1 - i] - der_x[iCtrlPtCount - i]
				* diagonal[iCtrlPtCount - 1 - i];
			der_y[iCtrlPtCount - 1 - i] = y[iCtrlPtCount - 1 - i] - der_y[iCtrlPtCount - i]
				* diagonal[iCtrlPtCount - 1 - i];
		}


		for (int i = 0; i < iCtrlPtCount - 1; i++)
		{


			double t = 1;
			Vec4f M_x_To_t = Vec4f(ptvCtrlPts[i + 0].x,
				ptvCtrlPts[i + 0].x + der_x[i] / 3,
				ptvCtrlPts[i + 1].x - der_x[i + 1] / 3,
				ptvCtrlPts[i + 1].x);
			M_x_To_t = Mb * M_x_To_t;


			//Vec4f M_y_To_t = Vec4f(ptvCtrlPts[i + 0].y,
			//	ptvCtrlPts[i + 0].y,
			//	der_y[i],
			//	der_y[i+1]);
			Vec4f M_y_To_t = Vec4f(ptvCtrlPts[i + 0].y,
				ptvCtrlPts[i + 0].y + der_y[i] / 3,
				ptvCtrlPts[i + 1].y - der_y[i + 1] / 3,
				ptvCtrlPts[i + 1].y);
			M_y_To_t = Mb * M_y_To_t;


			//cout << M_x_To_t[0] << " " << M_x_To_t[1] << " " << M_x_To_t[2] << " " << M_x_To_t[3] << endl;
			//cout << M_y_To_t[0] << " " << M_y_To_t[1] << " " << M_y_To_t[2] << " " << M_y_To_t[3] << endl;

			for (double t = 0.0; t <= 1; t += s_fFlatnessEpsilon)
			{
				Vec4f quadric = Vec4f(pow(t, 3), pow(t, 2), t, 1);
				float x = quadric * M_x_To_t;
				if (x > ptvCtrlPts[i + 1].x)
					x = ptvCtrlPts[i + 1].x - s_fFlatnessEpsilon;
				else if (x < ptvCtrlPts[i].x)
					x = ptvCtrlPts[i].x + s_fFlatnessEpsilon;
				ptvEvaluatedCurvePts.push_back(Point(x, quadric * M_y_To_t));
			}
			//cout << endl;
		}


		ptvEvaluatedCurvePts.push_back(ptvCtrlPts[0]);
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts[iCtrlPtCount - 1]);
		ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[iCtrlPtCount - 1].y));


		delete[] x;
		delete[] y;
		delete[] der_x;
		delete[] der_y;
		delete[] diagonal;
	}


}


#endif