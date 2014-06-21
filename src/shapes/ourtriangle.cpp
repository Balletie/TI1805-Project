#include "shapes.h"

OurTriangle::OurTriangle(Material& mat, Mesh *mesh, Triangle *triangle)
: Shape(mat, mesh->vertices[triangle->v[0]].p), _mesh(mesh), _triangle(triangle)
{}

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
	float d00 = Vec3Df::dotProduct(u, u);
	float d01 = Vec3Df::dotProduct(u, v);
	float d11 = Vec3Df::dotProduct(v, v);
	float d20 = Vec3Df::dotProduct(p - _origin, u);
	float d21 = Vec3Df::dotProduct(p - _origin, v);
	float invDenom = 1.0 / (d00 * d11 - d01 * d01);

	float a = (d11 * d20 - d01 * d21) * invDenom;
	if (a < -EPSILON) return false;

	float b = (d00 * d21 - d01 * d20) * invDenom;
	if (b < -EPSILON) return false;
	if (a + b > 1) return false;

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

void OurTriangle::draw() {

}
