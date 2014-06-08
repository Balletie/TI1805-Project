#ifndef __SHAPE_H__
#define __SHAPE_H__
#include <stdint.h>

#include "Vec3D.h"
#include <cstdint>

class Shape {
  public:
	Shape(Vec3Df, Vec3Df);
	virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&) = 0;
  	const Vec3Df _color;
	const Vec3Df _origin;
};

class Sphere : public Shape {
  public:
	Sphere(Vec3Df, Vec3Df, uint8_t);
	bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	const uint8_t _radius;
};

class Plane : public Shape {
  public:
	Plane(Vec3Df, Vec3Df, Vec3Df);
	bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	const Vec3Df _coeff;
};

#endif
