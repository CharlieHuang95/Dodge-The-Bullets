#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <list>
#include <map>
using namespace cv;
using namespace std;

// Settings
class Point_F {
public:
	std::map <char*, Point_F**> rotationDictionary;
	float x, y;
	std::vector<Point_F*> linkedJoints;
	bool adjusted = false;
	Point_F(double x_i = 0.0, double y_i = 0.0) {
		x = x_i;
		y = y_i;
	}
	void Link(Point_F*);
};

void Point_F::Link(Point_F* adjacent_Point) {
	linkedJoints.push_back(adjacent_Point);
}

class Line_F {
public:
	Point_F l_end, r_end;
	Line_F(Point_F l = 0.0, Point_F r = 0.0) {
		l_end = l;
		r_end = r;
	}
};

class Background {
public:
	Line_F ground;
	float left_end;
	float right_end;
};
