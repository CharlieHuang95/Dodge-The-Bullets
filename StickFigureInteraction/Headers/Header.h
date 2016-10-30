#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <list>
#include <map>
#include <cstdio>
#include <ctime>
#include "ImProcess.h"


bool trackObjects = true;
bool useMorphOps = true;
float SPEED = 0.04;
bool DEBUG = false;
bool useImage = false;
class StickFigure;
class Point_F;
class Line_F;
class Background;

#include "Rendering.h"
#include "StickFigure.h"
#include "Geometry.h"
#include "ImageProcessing.h"
#include "Projectile.h"

// Function Declarations
void DrawLine(Mat, Point_F, Point_F);
