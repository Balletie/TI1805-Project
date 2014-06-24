#include "shapes.h"

#include <stdio.h>
#include "float.h"

OurTriangle::OurTriangle(Material& mat, Mesh *mesh, Triangle *triangle)
: Shape(mat, mesh->vertices[triangle->v[0]].p), _mesh(mesh), _triangle(triangle)
{
	Vec3Df p1 = (*this)[1].p - (*this)[0].p;
	Vec3Df p2 = (*this)[2].p - (*this)[0].p;
	Vec3Df tc[3];
	for (int i = 0; i < 3; i++) {
		tc[i] = _mesh->texcoords[_triangle->t[i]];
	}
	float q1 = tc[1][0] - tc[0][0];
	float r1 = tc[1][1] - tc[0][1];
	float q2 = tc[2][0] - tc[0][0];
	float r2 = tc[2][1] - tc[0][1];

	float det = 1.f / (q1 * r2 - q2 * r1);
	tangent = Vec3Df(det * (r2 * p1[0] - r1 * p2[0]), det * (r2 * p1[1] - r1 * p2[1]), det * (r2 * p1[2] - r1 * p2[2]));
	bitangent = Vec3Df(det * (q1 * p2[0] - q2 * p1[0]), det * (q1 * p2[1] - q2 * p1[1]), det * (q1 * p2[2] - q2 * p1[2]));
	tangent.normalize();
	bitangent.normalize();
	truenormal = Vec3Df::crossProduct(tangent, bitangent);
	truenormal.normalize();
}

Vec3Df OurTriangle::shade(const Vec3Df& cam_pos, const Vec3Df& intersect, const Vec3Df& light_pos, const Vec3Df& normal) {
	if (!_mat.has_tex()) return Shape::shade(cam_pos, intersect, light_pos, normal);
	float u, v;
	float a, b;
	barycentric(intersect, a, b);
	Vec3Df texcoords[3];
	for (int i = 0; i < 3; i++) {
		texcoords[i] = _mesh->texcoords[_triangle->t[i]];
	}
	this->_tex->convertBarycentricToTexCoord(a, b, texcoords, u, v);
	Vec3Df diffuse = this->_tex->getColor(u,v);
	this->_mat.set_Kd(diffuse[0], diffuse[1], diffuse[2]);
	if (norm_set) {
		Vec3Df normal_tangent_space = this->_norm->getColor(u,v);
		Vec3Df mat_row_1 = Vec3Df(tangent[0], bitangent[0], truenormal[0]);
		Vec3Df mat_row_2 = Vec3Df(tangent[1], bitangent[1], truenormal[1]);
		Vec3Df mat_row_3 = Vec3Df(tangent[2], bitangent[2], truenormal[2]);

		Vec3Df norm_object_space = Vec3Df(	Vec3Df::dotProduct(mat_row_1, normal_tangent_space),
							Vec3Df::dotProduct(mat_row_2, normal_tangent_space),
							Vec3Df::dotProduct(mat_row_3, normal_tangent_space));
		return Shape::shade(cam_pos, intersect, light_pos, norm_object_space);
	} else {
		return Shape::shade(cam_pos, intersect, light_pos, normal);
	}
}
void OurTriangle::barycentric(const Vec3Df &p, float &a, float &b) {
	Vec3Df u = _mesh->vertices[_triangle->v[1]].p - _origin;
	Vec3Df v = _mesh->vertices[_triangle->v[2]].p - _origin;

	float d00 = Vec3Df::dotProduct(u, u);
	float d01 = Vec3Df::dotProduct(u, v);
	float d11 = Vec3Df::dotProduct(v, v);
	float d20 = Vec3Df::dotProduct(p - _origin, u);
	float d21 = Vec3Df::dotProduct(p - _origin, v);
	float invDenom = 1.0 / (d00 * d11 - d01 * d01);

	a = (d11 * d20 - d01 * d21) * invDenom;
	b = (d00 * d21 - d01 * d20) * invDenom;
}

bool OurTriangle::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal) {
	Vec3Df u = _mesh->vertices[_triangle->v[1]].p - _origin;
	Vec3Df v = _mesh->vertices[_triangle->v[2]].p - _origin;

	//
	// STEP 1: First calculate where the ray intersects the plane in which the triangle lies
	//
	// Calculate the normal of the plane
	Vec3Df planeNormal = Vec3Df::crossProduct(u, v);

	// Calculate the angle of the ray relative to the plane
	float denom = Vec3Df::dotProduct(dir,planeNormal);
	if (denom > -EPSILON && denom < EPSILON) return false;

	// Calculate term t in the expressen 'p = o + tD'
	float t = Vec3Df::dotProduct(_origin - origin, planeNormal) / denom;
	if (t < EPSILON) return false;

	Vec3Df p = origin + t * dir;

	//
	// STEP 2: Determine barycentric coordinates using Cramer's rule
	//
	float a, b;
	barycentric(p, a, b);
	if (a < -EPSILON || b < -EPSILON || a + b > 1) return false;

	//
	// STEP 3: Interpolate the vertex normals using barycentric coordinates
	//
	normal =	(1 - a - b) * _mesh->vertices[_triangle->v[0]].n +
				 a * _mesh->vertices[_triangle->v[1]].n +
				 b * _mesh->vertices[_triangle->v[2]].n;
	normal.normalize();
	new_origin = p;
	return true;
}

Vec3Df OurTriangle::getMidPoint() {
	return	_origin +
			1/3 * (_mesh->vertices[_triangle->v[1]].p - _origin) +
			1/3 * (_mesh->vertices[_triangle->v[2]].p - _origin);
}

void OurTriangle::draw() {

}

BoundingBox OurTriangle::getBoundingBox() {
	Vec3Df min(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3Df max = -min;

	// Loop over the 3 vertices that make up this triangle
	for (int i = 0; i < 3; i++) {
		// Loop over x, y, z
		for (int j = 0; j < 3; j++) {
			// Store the current coordinate value
			float cur = _mesh->vertices[_triangle->v[i]].p[j];
			if (cur < min[j]) min[j] = cur;
			if (cur > max[j]) max[j] = cur;
		}
	}
	return BoundingBox(min, max);
}

Vertex  OurTriangle::operator[] (int i) const {
	return _mesh->vertices[_triangle->v[i]];
}
