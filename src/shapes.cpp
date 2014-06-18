#include "shapes.h"
#include <stdio.h>
#if (defined _WIN32 || defined _WIN64)
#include <windows.h>
#endif
#include <GL/glut.h>

static const float EPSILON = 1e-4;

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

	// We use the following in place of the quadratic formula for
	// more numeric precision.
	float q = (b > 0) ?
				-0.5 * (b + sqrtf(disc)) :
				-0.5 * (b - sqrtf(disc));
	float t0 = q / a;
	float t1 = c / q;
	if (t0 < t1) std::swap(t0,t1);

	float t;
	if (t0 < EPSILON)	return false;
	if (t1 < 0)		t = t0;
	else			t = t1;

	normal = trans_origin + t * dir;
	normal.normalize();
	new_origin = origin + t * dir;
	return true;
}

void Sphere::draw() {
	glPushMatrix();

	glTranslatef(this->_origin[0],this->_origin[1],this->_origin[2]);
	glColor3f(this->_color[0],this->_color[1],this->_color[2]);
	glutSolidSphere(this->_radius, 20, 20);

	glPopMatrix();
}

bool Plane::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal)
{
	normal = _coeff;
	normal.normalize();

	float denom = Vec3Df::dotProduct(dir,normal);
	if (denom < 1e-4 && denom > -1e-4) return false;

	// Calculate term t in the expressen 'p = o + tD'
	float t = Vec3Df::dotProduct(_origin - origin, normal) / denom;
	if (t < EPSILON) return false;
	if (t < 1e-4) return false;

	new_origin = origin + t * dir;
	return true;
}

void Plane::draw() {
	glPushMatrix();

	glTranslatef(this->_origin[0],this->_origin[1],this->_origin[2]);
	glColor3f(this->_color[0],this->_color[1],this->_color[2]);
	glScalef(10,0.4,10);
	glutSolidCube(1);

	glPopMatrix();
}