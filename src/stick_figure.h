#pragma once

#include "stick_figure.cpp

#include <iostream>
#include <stdio.h>
#include <list>
#include "main.h"

enum SFState { WALK_FORWARD, WALK_BACKWARD, REPOSITION_F, REPOSITION_B };
enum StepState { RIGHT, LEFT };

class StickFigure {
public:
	Point_F l_arm, r_arm, l_leg, r_leg, l_hand, r_hand, l_foot, r_foot, head, center, pelvis, ref;
	std::vector<Point_F*> allJoints;
	SFState currentState = WALK_FORWARD;
	StepState forwardState = RIGHT;
	StepState backwardState = RIGHT;
	float size;

	void Set_Initial(int, Point_F, float);
	void rotate_point(Point_F*, Point_F*, float, char);
	void rotate_fixed(Point_F*, float, char);
	void reset_rotate();

	int l_stepForward();
	int r_stepForward();
	int l_stepBackward();
	int r_stepBackward();

	void assumePosition(int);
	void walkForward();
	void walkBackward();
	void followObject(Point);
	void FSM(Background);

	void shiftLocation(float, float);
	void adjustReference(Point_F, Point_F);
	void adjustAdjacentPoints(Point_F*, Point_F*, float, float);
	Mat Draw_Overlay(Mat);
};