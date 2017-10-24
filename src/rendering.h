#include "geometry.h"

#pragma once
void DrawLine(Mat image, Point_F point1_F, Point_F point2_F) {
	int thickness = 2;
	int lineType = LINE_8;
	Point point1 = Point(int(point1_F.x + 0.5), int(point1_F.y + 0.5));
	Point point2 = Point(int(point2_F.x + 0.5), int(point2_F.y + 0.5));
	line(image, point1, point2, Scalar(255, 255, 255), thickness, lineType);
}
