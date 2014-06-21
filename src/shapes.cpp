#include "shapes.h"
#include "Vertex.h"

#include <stdio.h>
#if (defined _WIN32 || defined _WIN64)
#include <windows.h>
#endif
#include <GL/glut.h>

Shape::Shape(Material& mat, Vec3Df org)
: _mat(mat), _origin(org)
{}

Vec3Df Shape::shade(const Vec3Df& cam_pos, const Vec3Df& intersect, const Vec3Df& light_pos, const Vec3Df& normal)  {
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

Vec3Df Checkerboard::shade(const Vec3Df& cam_pos, const Vec3Df& intersect, const Vec3Df& light_pos, const Vec3Df& normal) {
	int u = intersect[0]/checker_size;
	int v = intersect[1]/checker_size;
	int8_t u_even = u % 2;
	int8_t v_even = v % 2;
	int8_t mask_u = u_even >> 7;
	int8_t mask_v = v_even >> 7;
	uint8_t abs_u_even = (u_even ^ mask_u) - mask_u;
	uint8_t abs_v_even = (v_even ^ mask_v) - mask_v;
	float xor_bw = (float) (abs_u_even^abs_v_even);
	Vec3Df bw = xor_bw * Vec3Df(1.f,1.f,1.f);
	this->_mat.set_Kd(bw[0],bw[1],bw[2]);
	return Shape::shade(cam_pos, intersect, light_pos, normal);
}

Sphere::Sphere(Material& mat, Vec3Df org, float rad)
: Shape(mat, org), _radius(rad)
{}

Plane::Plane(Material& mat, Vec3Df org, Vec3Df coeff)
: Shape(mat, org), _coeff(coeff)
{}

Checkerboard::Checkerboard(Material& mat, Vec3Df org, Vec3Df coeff)
: Plane(mat, org, coeff)
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
