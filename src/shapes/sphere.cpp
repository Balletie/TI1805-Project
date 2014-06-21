#include "shapes.h"

#include <GL/glut.h>

Sphere::Sphere(Material& mat, Vec3Df org, float rad)
: Shape(mat, org), _radius(rad)
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
	glColor3f(this->_mat.Kd()[0],this->_mat.Kd()[1],this->_mat.Kd()[2]);
	glutSolidSphere(this->_radius, 20, 20);

	glPopMatrix();
}
