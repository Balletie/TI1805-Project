#include "shapes.h"
#include "Vertex.h"

#include <stdio.h>
#if (defined _WIN32 || defined _WIN64)
#include <windows.h>
#endif
#include <GL/glut.h>

static const float EPSILON = 1e-4;

Shape::Shape(Material& mat, Vec3Df org)
: _mat(mat), _origin(org)
{}

Vec3Df Shape::shade(const Vec3Df& cam_pos, const Vec3Df& intersect, const Vec3Df& light_pos, const Vec3Df& normal) {
	Vec3Df ambient(0.f,0.f,0.f);
	Vec3Df diffuse(0.f,0.f,0.f);
	Vec3Df specular(0.f,0.f,0.f);

	Vec3Df light_vec = light_pos - intersect;
	light_vec.normalize();

	if (this->_mat.has_Ka()) ambient = _mat.Ka();
	if (this->_mat.has_Kd()) {
		diffuse = Vec3Df::dotProduct(normal, light_vec) * _mat.Kd();
	}
	if(this->_mat.has_Ks()){
		Vec3Df reflect = 2 * Vec3Df::dotProduct(light_vec, normal) * normal - light_vec;
		Vec3Df view = cam_pos - intersect;
		view.normalize();

		if (Vec3Df::dotProduct(view, reflect) < 0) {
			specular = Vec3Df(0, 0, 0);
		} else {
			float shininess;
			if (this->_mat.has_Ns()) 	shininess = _mat.Ns();
			else				shininess = 21;
			specular = pow(Vec3Df::dotProduct(reflect, view), shininess) * _mat.Ks();
		}
	}
	return ambient + diffuse + specular;
}

Vec3Df Shape::refract(const Vec3Df &intersect, const Vec3Df &normal, const float dotProduct, const double ni1, const double ni2) {
	Vec3Df refract = Vec3Df(0.f, 0.f, 0.f);
	double ratio = ni1 / ni2;
	double root = 1 - ni1 * ni1 * (1 - dotProduct * dotProduct) / (ni2 * ni2);

	// If root < 0, total internal reflection takes place. In this case,
	// the refraction vector should be black. It already is, so do nothing then.
	if (root >= 0) {
		refract = ratio * (intersect - dotProduct * normal) - normal * sqrt(root);
	}

	return refract;
}

Sphere::Sphere(Material& mat, Vec3Df org, float rad)
: Shape(mat, org), _radius(rad)
{}

Plane::Plane(Material& mat, Vec3Df org, Vec3Df coeff)
: Shape(mat, org), _coeff(coeff)
{}

OurTriangle::OurTriangle(Material& mat, Mesh *mesh, Triangle *triangle)
: Shape(mat, mesh->vertices[triangle->v[0]].p), _mesh(mesh), _triangle(triangle)
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

bool Plane::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal)
{
	normal = _coeff;
	normal.normalize();

	float denom = Vec3Df::dotProduct(dir,normal);
	if (denom > -EPSILON && denom < EPSILON) return false;

	// Calculate term t in the expressen 'p = o + tD'
	float t = Vec3Df::dotProduct(_origin - origin, normal) / denom;
	if (t < EPSILON) return false;

	new_origin = origin + t * dir;
	return true;
}

void Plane::draw() {
	glPushMatrix();

	glTranslatef(this->_origin[0],this->_origin[1],this->_origin[2]);
	glColor3f(this->_mat.Kd()[0],this->_mat.Kd()[1],this->_mat.Kd()[2]);
	glScalef(10,0.4,10);
	glutSolidCube(1);

	glPopMatrix();
}

bool OurTriangle::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal) {
	Vec3Df u = _mesh->vertices[_triangle->v[1]].p - _origin;
	Vec3Df v = _mesh->vertices[_triangle->v[2]].p - _origin;

	//
	// STEP 1: First calculate where the ray intersects the plane in which the triangle lies
	//
	// Calculate the normal of the plane
	normal = Vec3Df::crossProduct(u, v);

	// Calculate the angle of the ray relative to the plane
	float denom = Vec3Df::dotProduct(dir,normal);
	if (denom > -EPSILON && denom < EPSILON) return false;

	// Calculate term t in the expressen 'p = o + tD'
	float t = Vec3Df::dotProduct(_origin - origin, normal) / denom;
	if (t < EPSILON) return false;

	Vec3Df p = origin + t * dir;

	//
	// STEP 2: Check whether the insersection point p lies to the left of all edges
	//
	float d00 = Vec3Df::dotProduct(u, u);
	float d01 = Vec3Df::dotProduct(u, v);
	float d11 = Vec3Df::dotProduct(v, v);
	float d20 = Vec3Df::dotProduct(p - _origin, u);
	float d21 = Vec3Df::dotProduct(p - _origin, v);
	float invDenom = 1.0 / (d00 * d11 - d01 * d01);

	float a = (d11 * d20 - d01 * d21) * invDenom ;
	if (a < -EPSILON) return false;

	float b = (d00 * d21 - d01 * d20) * invDenom ;
	if (b < -EPSILON) return false;
	if (a + b > 1) return false;

	normal =	(a * _mesh->vertices[_triangle->v[0]].n +
				 b * _mesh->vertices[_triangle->v[1]].n +
				 (1 - a - b) * _mesh->vertices[_triangle->v[2]].n);
	normal.normalize();
	new_origin = p;
	return true;
}

void OurTriangle::draw() {

}
