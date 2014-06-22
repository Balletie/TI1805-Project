#include "shapes.h"

#include "float.h"

OurTriangle::OurTriangle(Material& mat, Mesh *mesh, Triangle *triangle)
: Shape(mat, mesh->vertices[triangle->v[0]].p), _mesh(mesh), _triangle(triangle)
{}

void OurTriangle::barycentric(Vec3Df &p, float &a, float &b) {
	Vec3Df u = _mesh->vertices[_triangle->v[1]].p - _origin;
	Vec3Df v = _mesh->vertices[_triangle->v[2]].p - _origin;

}

bool OurTriangle::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal) {
	Vec3Df u = _mesh->vertices[_triangle->v[1]].p - _origin;
	Vec3Df v = _mesh->vertices[_triangle->v[2]].p - _origin;

	printf("------------------------\n");
	printf("v1: %f %f %f\n", _origin[0], _origin[1], _origin[2]);
	printf("v2: %f %f %f\n", _mesh->vertices[_triangle->v[1]].p[0], _mesh->vertices[_triangle->v[1]].p[1], _mesh->vertices[_triangle->v[1]].p[2]);
	printf("v3: %f %f %f\n", _mesh->vertices[_triangle->v[2]].p[0], _mesh->vertices[_triangle->v[2]].p[1], _mesh->vertices[_triangle->v[2]].p[2]);

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
	float d00 = Vec3Df::dotProduct(u, u);
	float d01 = Vec3Df::dotProduct(u, v);
	float d11 = Vec3Df::dotProduct(v, v);
	float d20 = Vec3Df::dotProduct(p - _origin, u);
	float d21 = Vec3Df::dotProduct(p - _origin, v);
	float invDenom = 1.0 / (d00 * d11 - d01 * d01);

	float a = (d11 * d20 - d01 * d21) * invDenom;
	float b = (d00 * d21 - d01 * d20) * invDenom;

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
