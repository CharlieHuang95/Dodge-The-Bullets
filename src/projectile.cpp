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

void ProjectileController::add_projectile(Point_F start_point, Point_F target_point, float myspeed, float mysize) {
	Projectile new_projectile = Projectile();
	new_projectile.Set_Initial(start_point, target_point, myspeed, mysize);
	projectiles.push_back(new_projectile);
}
void ProjectileController::update_projectiles() {
	for (size_t i = 0; i < projectiles.size(); i++) {
		projectiles[i].update_location();
	}
}
void ProjectileController::render_projectiles(Mat frame) {
	for (size_t i = 0; i < projectiles.size(); i++) {
		circle(frame, Point(projectiles[i].location.x, projectiles[i].location.y), projectiles[i].size, Scalar(255, 255, 255), 2);
	}
}
bool ProjectileController::check_collisions(float x, float y) {
	for (size_t i = 0; i < projectiles.size(); i++) {
		if (abs(y - projectiles[i].location.y) + abs(x - projectiles[i].location.x) < projectiles[i].size) {
			return true;
		}
	}
	return false;
}
