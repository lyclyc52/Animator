#include "Surface.h"
#include <algorithm>
#include "modelerdraw.h"


Surface::Surface()
{
	for (int i = 0; i < 4; i++)
	{
		std::vector < Point_3d > newLine;
		for (int j = 0; j < 4; j++)
		{
			newLine.push_back(Point_3d(i, j, i*j));
		}
		point_grid.push_back(newLine);
	}
}

int Surface::cY(int i)
{
	return min(yCtrlCount - 1, max(i, 0));
}

int Surface::cX(int i)
{
	return min(xCtrlCount - 1, max(i, 0));
}

void Surface::evaluateSurface()
{
	Mat4f M_B = Mat4f(-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 0, 3, 0,
		1, 4, 1, 0
	);
	M_B = M_B / 6;

	xCtrlCount = point_grid.size();
	yCtrlCount = point_grid[0].size();
	evaluated_points.clear();
	int start = 0;
	int count = 0;
	for (int i = -3; i < xCtrlCount - 1; i++)
	{

		start += count;
		cout << start << endl;
		for (int j = -3; j < yCtrlCount - 1; j++)
		{
			count = 0;
			Mat4f centerX =
				Mat4f(point_grid[cX(i + 0)][cY(j + 0)].x, point_grid[cX(i + 0)][cY(j + 1)].x, point_grid[cX(i + 0)][cY(j + 2)].x, point_grid[cX(i + 0)][cY(j + 3)].x,
					point_grid[cX(i + 1)][cY(j + 0)].x, point_grid[cX(i + 1)][cY(j + 1)].x, point_grid[cX(i + 1)][cY(j + 2)].x, point_grid[cX(i + 1)][cY(j + 3)].x,
					point_grid[cX(i + 2)][cY(j + 0)].x, point_grid[cX(i + 2)][cY(j + 2)].x, point_grid[cX(i + 2)][cY(j + 2)].x, point_grid[cX(i + 2)][cY(j + 3)].x,
					point_grid[cX(i + 3)][cY(j + 0)].x, point_grid[cX(i + 3)][cY(j + 3)].x, point_grid[cX(i + 3)][cY(j + 2)].x, point_grid[cX(i + 3)][cY(j + 3)].x);

			Mat4f centerY =
				Mat4f(point_grid[cX(i + 0)][cY(j + 0)].y, point_grid[cX(i + 0)][cY(j + 1)].y, point_grid[cX(i + 0)][cY(j + 2)].y, point_grid[cX(i + 0)][cY(j + 3)].y,
					point_grid[cX(i + 1)][cY(j + 0)].y, point_grid[cX(i + 1)][cY(j + 1)].y, point_grid[cX(i + 1)][cY(j + 2)].y, point_grid[cX(i + 1)][cY(j + 3)].y,
					point_grid[cX(i + 2)][cY(j + 0)].y, point_grid[cX(i + 2)][cY(j + 1)].y, point_grid[cX(i + 2)][cY(j + 2)].y, point_grid[cX(i + 2)][cY(j + 3)].y,
					point_grid[cX(i + 3)][cY(j + 0)].y, point_grid[cX(i + 3)][cY(j + 1)].y, point_grid[cX(i + 3)][cY(j + 2)].y, point_grid[cX(i + 3)][cY(j + 3)].y);

			Mat4f centerZ =
				Mat4f(point_grid[cX(i + 0)][cY(j + 0)].z, point_grid[cX(i + 0)][cY(j + 1)].z, point_grid[cX(i + 0)][cY(j + 2)].z, point_grid[cX(i + 0)][cY(j + 3)].z,
					point_grid[cX(i + 1)][cY(j + 0)].z, point_grid[cX(i + 1)][cY(j + 1)].z, point_grid[cX(i + 1)][cY(j + 2)].z, point_grid[cX(i + 1)][cY(j + 3)].z,
					point_grid[cX(i + 2)][cY(j + 0)].z, point_grid[cX(i + 2)][cY(j + 1)].z, point_grid[cX(i + 2)][cY(j + 2)].z, point_grid[cX(i + 2)][cY(j + 3)].z,
					point_grid[cX(i + 3)][cY(j + 0)].z, point_grid[cX(i + 3)][cY(j + 1)].z, point_grid[cX(i + 3)][cY(j + 2)].z, point_grid[cX(i + 3)][cY(j + 3)].z);

			Mat4f transformX = M_B * centerX * M_B.transpose();
			Mat4f transformY = M_B * centerY * M_B.transpose();
			Mat4f transformZ = M_B * centerZ * M_B.transpose();


			for (double u = 0; u < 1; u += s_fFlatnessEpsilon)
			{
				Vec4f quadric_u = Vec4f(pow(u, 3), pow(u, 2), u, 1);
				if (j == -3)
				{
					std::vector<Point_3d> newline;
					evaluated_points.push_back(newline);
				}
				for (double v = 0; v < 1; v += s_fFlatnessEpsilon)
				{
					Vec4f quadric_v = Vec4f(pow(v, 3), pow(v, 2), v, 1);
					evaluated_points[start + count].push_back(Point_3d(quadric_u * transformX * quadric_v,
						quadric_u * transformY * quadric_v,
						quadric_u * transformZ * quadric_v));
				}
				count++;
			}

		}


	}
}


void Surface::drawSurface()
{
	if (!evaluated_points.empty())
	{
		int Ylength = (evaluated_points[0]).size();
		int Xlength = evaluated_points.size();
		for (int i = 0; i < Xlength - 1; i++)
		{
			for (int j = 0; j < Ylength - 1; j++)
			{
				drawTriangle(evaluated_points[i][j].x, evaluated_points[i][j].y, evaluated_points[i][j].z,
					evaluated_points[i][j + 1].x, evaluated_points[i][j + 1].y, evaluated_points[i][j + 1].z,
					evaluated_points[i + 1][j].x, evaluated_points[i + 1][j].y, evaluated_points[i + 1][j].z);
				drawTriangle(evaluated_points[i+1][j+1].x, evaluated_points[i+1][j+1].y, evaluated_points[i+1][j+1].z,
					evaluated_points[i][j + 1].x, evaluated_points[i][j + 1].y, evaluated_points[i][j + 1].z,
					evaluated_points[i + 1][j].x, evaluated_points[i + 1][j].y, evaluated_points[i + 1][j].z);
			
			}
		}
	}

}


