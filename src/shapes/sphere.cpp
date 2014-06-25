#include "shapes.h"

#include <GL/glut.h>
#include <math.h>

#define _USE_MATH_DEFINES
#define M_PI 3.14159265358979323846


Sphere::Sphere(Material& mat, Vec3Df org, float rad)
: Shape(mat, org), _radius(rad)
{}

Vec3Df Sphere::shade(const Vec3Df& cam_pos, const Vec3Df& intersect, const Vec3Df& light_pos, const Vec3Df& normal){
	if (!_mat.has_tex()) return Shape::shade(cam_pos, intersect, light_pos, normal);
	float u, v;
	Vec3Df mid = this->_origin;
	Vec3Df dir = intersect - mid;
	dir.normalize();
	u = 0.5 + (atan2(dir[2], dir[0]))/(2*M_PI);
	v = 0.5 - asin(dir[1])/M_PI;
	Vec3Df diffuse = this->_tex->getColor(u,v);
	this->_mat.set_Kd(diffuse[0], diffuse[1], diffuse[2]);
	return Shape::shade(cam_pos, intersect, light_pos, normal);
}

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
