#include "projectile.h"

void Projectile::Set_Initial(Point_F start_point, Point_F target_point, float myspeed, float mysize) {
	location = start_point;
	slope = (target_point.y - start_point.y) / (target_point.x - start_point.x);
	if (target_point.x - start_point.x < 0) { backward = true; }
	speed = myspeed;
	size = mysize;
}

void Projectile::update_location(void) {
	float speedc = speed;
	if (backward) { speedc = -speed; }
	location.x = location.x + speedc;
	location.y = location.y + slope*speedc;
}

bool Projectile::check_collision(float x, float y) {
	if (abs(y - location.y) + abs(x - location.x) < size) {
		return true;
	}
	return false;
}
