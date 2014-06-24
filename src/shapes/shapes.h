#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <stdint.h>

#include "../mesh.h"
#include "../Vec3D.h"
#include "../image.h"
#include "boundingbox.h"

static const float EPSILON = 1e-4;

class OurObject {
  public:
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
	virtual Vec3Df shade(const Vec3Df&, const Vec3Df&, const Vec3Df&, const Vec3Df&) = 0;

	/**
	 * Calculate the refraction vector. For simplicity, all vectors must be normalized.
	 * It is assumed we are either inside an object or in air.
	 * @param normal The normal at the point of intersection.
	 * @param dir The direction of the view vector.
	 * @param ni1 The other refraction index.
	 * @param fresnel The return address for the fresnel value.
	 */
	virtual Vec3Df refract(const Vec3Df&, const Vec3Df&, const float&, float &fresnel) = 0;

	virtual bool hasMat() = 0;
	virtual Material& getMat() = 0;
};

class Shape : public OurObject {
  public:
	Shape(Material& mat, Vec3Df org);

	/**
	 * Shade the object using specular, diffuse and ambient terms of the Material.
	 * @param cam_pos The camera position
	 * @param intersect The point of intersection with this object and the ray.
	 * @param light_pos The position of the light.
	 * @param normal The normal at the point of intersection
	 * @return The color for this intersection point.
	 */
	virtual Vec3Df shade(const Vec3Df&, const Vec3Df&, const Vec3Df&, const Vec3Df&);

	/**
	 * Calculate the refraction vector. For simplicity, all vectors must be normalized.
	 * It is assumed we are either inside an object or in air.
	 * @param normal The normal at the point of intersection.
	 * @param dir The direction of the view vector.
	 * @param ni1 The other refraction index.
	 * @param fresnel The return address for the fresnel value.
	 */
	Vec3Df refract(const Vec3Df&, const Vec3Df&, const float&, float &fresnel);

	virtual bool hasMat() { return true; }
	virtual Material& getMat() { return _mat; }
	bool hasTexture() { return texture_set; }
	void setTexture(Texture* tex) { texture_set = true; _tex = tex; }
	/**
	 * Draw the object using GLUT functions and OpenGL.
	 * NOTE: This has nothing to do with raytracing the object!
	 */
	virtual void draw() = 0;

	bool texture_set;
	Texture* _tex;
	Material& _mat;
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

class Checkerboard : public Plane {
  public:
	Checkerboard(Material& mat, Vec3Df org, Vec3Df coeff);
	virtual Vec3Df shade(const Vec3Df&, const Vec3Df&, const Vec3Df&, const Vec3Df&);
  private:
	const float checker_size = 2.f;
};

class OurTriangle : public Shape {
  public:
	OurTriangle(Material& mat, Mesh *mesh, Triangle *triangle);
	virtual Vec3Df shade(const Vec3Df&, const Vec3Df&, const Vec3Df&, const Vec3Df&);
	void barycentric(const Vec3Df &p, float &a, float &b);
	virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
	virtual void draw();
	BoundingBox getBoundingBox();
	Vec3Df getMidPoint();
	const Mesh* _mesh;
	const Triangle* _triangle;
	Vertex  operator[] (int i) const;
};

#endif
