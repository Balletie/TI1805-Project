#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <stdint.h>
#include "mesh.h"
#include "Vec3D.h"

class Shape {
  public:
	Shape(Vec3Df color, Vec3Df specular, Vec3Df org);

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
	 * Draw the object using GLUT functions and OpenGL.
	 * NOTE: This has nothing to do with raytracing the object!
	 */
	virtual void draw() = 0;

  	const Vec3Df _color;
  	const Vec3Df _specular;
	const Vec3Df _origin;
};

class Sphere : public Shape {
  public:
	Sphere(Vec3Df color, Vec3Df specular, Vec3Df org, float rad);

	virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	virtual void draw();
	const float _radius;
};

class Plane : public Shape {
  public:
	Plane(Vec3Df color, Vec3Df specular, Vec3Df org, Vec3Df coeff);
	virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	virtual void draw();
	const Vec3Df _coeff;
};

class OurTriangle : public Shape {
  public:
	OurTriangle(Vec3Df color, Vec3Df specular, Mesh *mesh, Triangle *triangle);
	virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	virtual void draw();
	const Mesh* _mesh;
	const Triangle* _triangle;
};

#endif
