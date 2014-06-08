#include "shapes.h"

Shape::Shape(Vec3Df color, Vec3Df org)
: _color(color), _origin(org)
{}

Sphere::Sphere(Vec3Df color, Vec3Df org, uint8_t rad)
: Shape(color, org), _radius(rad)
{}

Plane::Plane(Vec3Df color, Vec3Df org, Vec3Df coeff)
: Shape(color, org), _coeff(coeff)
{}

bool Sphere::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal) {
}

bool Plane::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal) {
}
