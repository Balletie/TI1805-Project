#include "shapes.h"

#include <stdio.h>

Shape::Shape(Vec3Df color, Vec3Df specular, Vec3Df org)
: _color(color), _specular(specular), _origin(org)
{}

Sphere::Sphere(Vec3Df color, Vec3Df specular, Vec3Df org, float rad)
: Shape(color, specular, org), _radius(rad)
{}

Plane::Plane(Vec3Df color, Vec3Df specular, Vec3Df org, Vec3Df coeff)
: Shape(color, specular, org), _coeff(coeff)
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

	normal = trans_origin + t*dir;
	normal.normalize();
	new_origin = origin + t * dir;
	return true;
}

bool Plane::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal)
{
	normal = _coeff;
	normal.normalize();

	float denom = Vec3Df::dotProduct(dir,normal);
	if (denom >= 0) return false;

	// Calculate term t in the expressen 'p = o + tD'
	float t = Vec3Df::dotProduct(_origin - origin, normal) / denom;

	new_origin = origin + t * dir;
	return true;
}
