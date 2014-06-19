#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <stdint.h>
#include "mesh.h"
#include "Vec3D.h"

class Shape {
  public:
	Shape(Material& mat, Vec3Df org);

	/**
	 * Check if a ray intersects with this object.
	 * @param origin Where the ray came from.
	 * @param dir The direction of the ray.
	 * @param new_origin The closest positive intersection point. Acts like return value.
	 * @param normal The normal at the point of intersection. Acts like return value.
	 * @return Whether the ray intersect with this object.
	 */
	virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&) = 0;

	/**
	 * Shade the object using specular, diffuse and ambient terms of the Material.
	 * @param cam_pos The camera position
	 * @param intersect The point of intersection with this object and the ray.
	 * @param light_pos The position of the light.
	 * @param normal The normal at the point of intersection
	 * @return The color for this intersection point.
	 */
	Vec3Df shade(const Vec3Df&, const Vec3Df&, const Vec3Df&, const Vec3Df&);

	/**
	 * Draw the object using GLUT functions and OpenGL.
	 * NOTE: This has nothing to do with raytracing the object!
	 */
	virtual void draw() = 0;

	const Material& _mat;
	const Vec3Df _origin;
};

class Sphere : public Shape {
  public:
	Sphere(Material& mat, Vec3Df org, float rad);

	virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	virtual void draw();
	const float _radius;
};

class Plane : public Shape {
  public:
	Plane(Material& mat, Vec3Df org, Vec3Df coeff);
	virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	virtual void draw();
	const Vec3Df _coeff;
};

class OurTriangle : public Shape {
  public:
	OurTriangle(Material& mat, Mesh *mesh, Triangle *triangle);
	virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	virtual void draw();
	const Mesh* _mesh;
	const Triangle* _triangle;
};

#endif
