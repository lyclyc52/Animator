#pragma once

#include "vec.h"
#include <vector>
#include "mat.h"
#include "camera.h"
#include "Point_3D.h"
class Surface
{
public:
	Surface();
	std::vector< std::vector<Point_3d> > point_grid;
	float const s_fFlatnessEpsilon = 0.1;
	std::vector< std::vector<Point_3d> > evaluated_points;
	int xCtrlCount;
	int yCtrlCount;
	void evaluateSurface();
	int cY(int);
	int cX(int);
	void drawSurface();

	int getXN() { return point_grid.size(); };
	int getYN() { return point_grid[0].size(); };
};