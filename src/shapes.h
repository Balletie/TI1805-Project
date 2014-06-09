#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <stdint.h>
#include "Vec3D.h"

class Shape {
  public:
	Shape(Vec3Df color, Vec3Df specular, Vec3Df org);
	virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&) = 0;
	virtual void draw() = 0;

  	const Vec3Df _color;
  	const Vec3Df _specular;
	const Vec3Df _origin;
};

class Sphere : public Shape {
  public:
	Sphere(Vec3Df color, Vec3Df specular, Vec3Df org, float rad);
	bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	void draw();
	const float _radius;
};

class Plane : public Shape {
  public:
	Plane(Vec3Df color, Vec3Df specular, Vec3Df org, Vec3Df coeff);
	bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	void draw();
	const Vec3Df _coeff;
};

#endif
