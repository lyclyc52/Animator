#pragma once
#ifndef POINT_3D_H_INCLUDED
#define POINT_3D_H_INCLUDED

#pragma warning(disable : 4786)

#include <functional>
#include <iostream>
#include <cmath>

//using namespace std;

class Point_3d
{
public:

	Point_3d(void)
	{
		x = y = z = 0.0;
	}
	Point_3d(const float& x, const float& y, const float& z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	};



	float x;
	float y;
	float z;
};


class Point3dSmallerXCompare : public std::binary_function<const Point_3d&, const Point_3d&, bool>
{
public:
	bool operator()(const Point_3d& first, const Point_3d& second) const
	{
		bool hasSmallerX = (first.x < second.x);

		return hasSmallerX;
	}
};

class Point3dLargerXCompare : public std::binary_function<const Point_3d&, const Point_3d&, bool>
{
public:
	bool operator()(const Point_3d& first, const Point_3d& second) const
	{
		bool hasLargerX = (first.x > second.x);

		return hasLargerX;
	}
};

class Point3dSmallerYCompare : public std::binary_function<const Point_3d&, const Point_3d&, bool>
{
public:
	bool operator()(const Point_3d& first, const Point_3d& second) const
	{
		bool hasSmallerY = (first.y < second.y);

		return hasSmallerY;
	}
};

class Point3dLargerYCompare : public std::binary_function<const Point_3d&, const Point_3d&, bool>
{
public:
	bool operator()(const Point_3d& first, const Point_3d& second) const\
	{
		bool hasLargerY = (first.y > second.y);

		return hasLargerY;
	}
};

#endif // POINT_H_INCLUDED
