#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <list>
#include <map>

class StickFigure;
class Point_F;
class Line_F;
class Background;

#include "Rendering.h"
#include "StickFigure.h"
#include "Geometry.h"
#include "ImageProcessing.h"

// Function Declarations
void DrawLine(Mat, Point_F, Point_F);