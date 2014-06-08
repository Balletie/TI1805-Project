#include "shapes.h"

Shape::Shape(Vec3Df color, Vec3Df org)
: _color(color), _origin(org)
{}

Sphere::Sphere(Vec3Df color, Vec3Df org, uint8_t rad)
: Shape(color, org), _radius(rad)
{}

Plane::Plane(Vec3Df color, Vec3Df org, Vec3Df coeff)
: Shape(color, org), _coeff(coeff)
{}

bool Sphere::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal) {
	Vec3Df trans_origin = origin - this->_origin;
	float a = Vec3Df::dotProduct(dir, dir);
	float b = 2 * Vec3Df::dotProduct(trans_origin, dir);
	float c = Vec3Df::dotProduct(trans_origin, trans_origin) - this->_radius * this->_radius;

	float disc = b * b - 4 * a * c;
	if (disc < 0)	return false;

	float t0 = (-b + sqrtf(disc)) / (2 * a);
	float t1 = (-b - sqrtf(disc)) / (2 * a);
	float t  = t0 < t1 ? t0 : t1;
	if (t < 0)	return false;

	new_origin = origin + t * dir;
	return true;
}

bool Plane::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal) {
}
