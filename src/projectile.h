#pragma once
#include <iostream>
#include <stdio.h>
#include <list>
#include <cmath>
#include "main.h"

using namespace cv;
using namespace std;

class Projectile {
public:
	Point_F location;
	float slope;
	float speed;
	float size;
	bool backward;
	void Set_Initial(Point_F, Point_F, float, float);
	void update_location(void);
	bool check_collision(float, float);
};

class ProjectileController {
public:
	vector<Projectile> projectiles;
	void add_projectile(Point_F, Point_F, float, float);
	void update_projectiles(void);
	void render_projectiles(Mat);
	bool check_collisions(float, float);
};
