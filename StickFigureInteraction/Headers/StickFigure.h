#include <iostream>
#include <stdio.h>
#include <list>
#include <map>
#include "Geometry.h"

using namespace cv;
using namespace std;

enum SFState { WALK_FORWARD, WALK_BACKWARD, REPOSITION_F, REPOSITION_B };
enum StepState { RIGHT, LEFT };

class StickFigure {
public:
	Point_F l_arm, r_arm, l_leg, r_leg, l_hand, r_hand, l_foot, r_foot, head, center, pelvis, ref;
	Point_F* fixed_point;
	SFState currentState = WALK_FORWARD;
	StepState forwardState = RIGHT;
	StepState backwardState = RIGHT;
	float size;
	Point_F background_size;
	std::vector<Point_F*> allJoints;
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
	void FSM(Background);
	void shiftLocation(float, float);
	void adjustReference(Point_F, Point_F);
	void adjustAdjacentPoints(Point_F*, Point_F*, float, float);
	Mat Draw_Overlay(Mat);
};

void StickFigure::Set_Initial(int height, Point_F my_center, float my_size) {
	size = my_size;
	center = Point_F(my_center.x, height - my_center.y);
	head = Point_F(center.x, center.y - size / 2);
	pelvis = Point_F(center.x, center.y + size);
	l_leg = Point_F(pelvis.x - size / 2, pelvis.y + size / 2);
	l_foot = Point_F(l_leg.x, l_leg.y + size * 2 / 3);
	r_leg = Point_F(pelvis.x + size / 2, pelvis.y + size / 2);
	r_foot = Point_F(r_leg.x, r_leg.y + size * 2 / 3);
	l_arm = Point_F(center.x - size / 2, center.y + size / 2);
	l_hand = Point_F(l_arm.x, l_arm.y + size / 2);
	r_arm = Point_F(center.x + size / 2, center.y + size / 2);
	r_hand = Point_F(r_arm.x, r_arm.y + size / 2);
	ref = Point_F(pelvis.x, r_foot.y);

	allJoints.push_back(&center);
	allJoints.push_back(&head);
	allJoints.push_back(&pelvis);
	allJoints.push_back(&l_leg);
	allJoints.push_back(&l_foot);
	allJoints.push_back(&r_leg);
	allJoints.push_back(&r_foot);
	allJoints.push_back(&l_arm);
	allJoints.push_back(&l_hand);
	allJoints.push_back(&r_arm);
	allJoints.push_back(&r_hand);

	head.Link(&center); center.Link(&head);
	r_hand.Link(&r_arm); r_arm.Link(&r_hand);
	r_arm.Link(&center); center.Link(&r_arm);
	l_arm.Link(&center); center.Link(&l_arm);
	l_arm.Link(&l_hand); l_hand.Link(&l_arm);
	center.Link(&pelvis); pelvis.Link(&center);
	l_leg.Link(&pelvis); pelvis.Link(&l_leg);
	r_leg.Link(&pelvis); pelvis.Link(&r_leg);
	l_leg.Link(&l_foot); l_foot.Link(&l_leg);
	r_leg.Link(&r_foot); r_foot.Link(&r_leg);
}

Mat StickFigure::Draw_Overlay(Mat oriImage) {
	Mat image = oriImage.clone();
	circle(image, Point(int(head.x + 0.5), int(head.y + 0.5)), 10, Scalar(255, 255, 255), 2);
	DrawLine(image, center, pelvis);
	DrawLine(image, pelvis, l_leg);
	DrawLine(image, pelvis, r_leg);
	DrawLine(image, l_leg, l_foot);
	DrawLine(image, r_leg, r_foot);
	DrawLine(image, center, l_arm);
	DrawLine(image, center, r_arm);
	DrawLine(image, l_arm, l_hand);
	DrawLine(image, r_arm, r_hand);
	DrawLine(image, center, head);
	return image;
}

void StickFigure::rotate_point(Point_F* p, Point_F* pivot, float angle, char direction) {

	float s = sin(angle);
	float c = cos(angle);
	float x_tmp, y_tmp;
	p->x -= pivot->x;
	p->y -= pivot->y;

	// 'c' for clockwise, counterclockwise otherwise
	if (direction == 'c') {
		x_tmp = p->x * c - p->y * s;
		y_tmp = p->x * s + p->y * c;
	}
	else {
		x_tmp = p->y * s + p->x * c;
		y_tmp = p->y * c - p->x * s;
	}

	float x_diff = x_tmp - p->x;
	float y_diff = y_tmp - p->y;
	p->x = x_tmp + pivot->x;
	p->y = y_tmp + pivot->y;
	p->adjusted = true;

	// Need to adjust all points except the pivot and its adjacent points
	adjustAdjacentPoints(pivot, p, x_diff, y_diff);
	reset_rotate();

}

void StickFigure::adjustAdjacentPoints(Point_F* fixedPoint, Point_F* p, float x_diff, float y_diff) {
	for (int i = 0; i < p->linkedJoints.size(); i++) {
		if (p->adjusted != true) {
			p->x += x_diff;
			p->y += y_diff;
		}
		p->adjusted = true;
		if (p->linkedJoints[i] == fixedPoint) { continue; }
		if (p->linkedJoints[i]->adjusted == true) { continue; }
		adjustAdjacentPoints(fixedPoint, p->linkedJoints[i], x_diff, y_diff);
		//rotate_fixed(pivot->linkedJoints[i], angle);
	}
}

void StickFigure::rotate_fixed(Point_F* pivot, float angle, char c) {
	for (int i = 0; i < pivot->linkedJoints.size(); i++) {
		if (pivot->linkedJoints[i]->adjusted == true) { continue; }
		rotate_point(pivot->linkedJoints[i], pivot, angle, c);
		//rotate_fixed(pivot->linkedJoints[i], angle);
	}
}

void StickFigure::reset_rotate() {
	center.adjusted = false;
	head.adjusted = false;
	pelvis.adjusted = false;
	l_leg.adjusted = false;
	l_foot.adjusted = false;
	r_leg.adjusted = false;
	r_foot.adjusted = false;
	l_arm.adjusted = false;
	l_hand.adjusted = false;
	r_arm.adjusted = false;
	r_hand.adjusted = false;
}

int StickFigure::l_stepForward() {
	reset_rotate();
	if (pelvis.x <= r_foot.x + size / 2) {
		rotate_point(&l_hand, &l_arm, SPEED * 6, 'n');
		//rotate_point(&r_hand, &r_arm, SPEED*6, 'n');
		rotate_point(&l_arm, &center, SPEED * 6, 'n');
		rotate_point(&r_arm, &center, SPEED * 6, 'c');
		rotate_point(&r_leg, &r_foot, SPEED, 'c');
		rotate_point(&pelvis, &r_leg, SPEED * 6, 'c');
		rotate_point(&l_leg, &pelvis, SPEED * 6, 'n');
		rotate_point(&l_foot, &l_leg, SPEED, 'n');
		return 1;
	}
	else { return 0; }
}

int StickFigure::r_stepForward() {
	reset_rotate();
	if (pelvis.x <= l_foot.x + size / 2) {
		rotate_point(&l_hand, &l_arm, SPEED * 6, 'c');
		//rotate_point(&r_hand, &r_arm, SPEED*6, 'c');

		rotate_point(&l_arm, &center, SPEED * 6, 'c');
		rotate_point(&r_arm, &center, SPEED * 6, 'n');
		rotate_point(&l_leg, &l_foot, SPEED, 'c');
		rotate_point(&pelvis, &l_leg, SPEED * 6, 'c');
		rotate_point(&r_leg, &pelvis, SPEED * 6, 'n');
		rotate_point(&r_foot, &r_leg, SPEED, 'n');
		return 0;
	}
	else { return 1; }
}

int StickFigure::r_stepBackward() {
	reset_rotate();
	if (pelvis.x >= l_foot.x - size / 2) {
		rotate_point(&l_hand, &l_arm, SPEED * 6, 'c');

		rotate_point(&l_arm, &center, SPEED * 6, 'n');
		rotate_point(&r_arm, &center, SPEED * 6, 'c');
		rotate_point(&l_leg, &l_foot, SPEED, 'n');
		rotate_point(&pelvis, &l_leg, SPEED * 6, 'n');
		rotate_point(&r_leg, &pelvis, SPEED * 6, 'c');
		rotate_point(&r_foot, &r_leg, SPEED, 'c');
		return 0;
	}
	else { return 1; }
}

int StickFigure::l_stepBackward() {
	reset_rotate();
	if (pelvis.x >= r_foot.x - size / 2) {
		rotate_point(&l_hand, &l_arm, SPEED * 6, 'n');

		rotate_point(&l_arm, &center, SPEED * 6, 'c');
		rotate_point(&r_arm, &center, SPEED * 6, 'n');
		rotate_point(&r_leg, &r_foot, SPEED, 'n');
		rotate_point(&pelvis, &r_leg, SPEED * 6, 'n');
		rotate_point(&l_leg, &pelvis, SPEED * 6, 'c');
		rotate_point(&l_foot, &l_leg, SPEED, 'c');
		return 1;
	}
	else { return 0; }
}

void StickFigure::shiftLocation(float x, float y) {
	center.x += x; center.y += y;
	head.x += x; head.y += y;
	pelvis.x += x; pelvis.y += y;
	l_leg.x += x; l_leg.y += y;
	l_foot.x += x; l_foot.y += y;
	r_leg.x += x; r_leg.y += y;
	r_foot.x += x; r_foot.y += y;
	l_arm.x += x; l_arm.y += y;
	l_hand.x += x; l_hand.y += y;
	r_arm.x += x; r_arm.y += y;
	r_hand.x += x; r_hand.y += y;
	ref.x = center.x; ref.y += y;
}

void StickFigure::adjustReference(Point_F referencePoint2, Point_F referencePoint) {
	Point_F ref_low;
	Point_F ref_high;
	if (referencePoint.x < referencePoint2.x) { ref_low = referencePoint; ref_high = referencePoint2; }
	else { ref_low = referencePoint2; ref_high = referencePoint; }
	float x_diff = ref.x - ref_low.x;
	float slope = (ref_high.y - ref_low.y) / (ref_high.x - ref_low.x);
	float new_y = ref_low.y + x_diff * slope;
	float y_diff = new_y - ref.y;
	if (DEBUG) { cout << "ref_low = " << ref_low.y << ", ref = " << ref.y << "slope = " << slope << "new_y = " << new_y << "\n"; }
	shiftLocation(0, y_diff);
}

void StickFigure::walkForward() {
	switch (forwardState) {
	case RIGHT:
		if (l_stepForward() == 0) { forwardState = LEFT; }
		break;
	case LEFT:
		if (r_stepForward() == 1) { forwardState = RIGHT; }
		break;
	}
}

void StickFigure::walkBackward() {
	switch (backwardState) {
	case RIGHT:
		if (l_stepBackward() == 0) { backwardState = LEFT; }
		break;
	case LEFT:
		if (r_stepBackward() == 1) { backwardState = RIGHT; }
		break;
	}
}

void StickFigure::assumePosition(int position) {
	switch (position) {
	case 0:
		head.x = center.x; head.y = center.y - size / 2;
		pelvis.x = center.x; pelvis.y = center.y + size;
		l_leg.x = pelvis.x - size / 2; l_leg.y = pelvis.y + size / 2;
		l_foot.x = l_leg.x; l_foot.y = l_leg.y + size * 2 / 3;
		r_leg.x = pelvis.x + size / 2; r_leg.y = pelvis.y + size / 2;
		r_foot.x = r_leg.x; r_foot.y = r_leg.y + size * 2 / 3;
		l_arm.x = center.x - size / 2; l_arm.y = center.y + size / 2;
		l_hand.x = l_arm.x; l_hand.y = l_arm.y + size / 2;
		r_arm.x = center.x + size / 2; r_arm.y = center.y + size / 2;
		r_hand.x = r_arm.x; r_hand.y = r_arm.y + size / 2;
		ref.x = pelvis.x; ref.y = r_foot.y;
		break;
	}
}

void StickFigure::FSM(Background back) {
	switch (currentState) {
	case WALK_FORWARD:
		walkForward();
		if (center.x >= back.right_end) { currentState = REPOSITION_B; }
		break;
	case WALK_BACKWARD:
		walkBackward();
		if (center.x <= back.left_end) { currentState = REPOSITION_F; }
		break;
	case REPOSITION_F:
		assumePosition(0);
		currentState = WALK_FORWARD;
		break;
	case REPOSITION_B:
		assumePosition(0);
		currentState = WALK_BACKWARD;
	}
}