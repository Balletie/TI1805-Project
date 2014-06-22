#ifndef KDNODE_H_
#define KDNODE_H_

#include <vector>
#include "float.h"

#include "../Vec3D.h"

enum AXES {
	X,
	Y,
	Z
};

// FORWARD DECLARATION
class OurTriangle;
class Shape;

class BoundingBox {
public:
	BoundingBox(Vec3Df min = Vec3Df(FLT_MAX, FLT_MAX, FLT_MAX), Vec3Df max = Vec3Df(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	void expand(BoundingBox box);
	int longestAxis();
	bool intersect(const Vec3Df& origin, const Vec3Df& dir);
	Vec3Df _min;
	Vec3Df _max;
};

class KDNode {
public:
	BoundingBox bbox;
	KDNode* left;
	KDNode* right;
	std::vector<OurTriangle*> triangles;

	KDNode();
	virtual ~KDNode();

	KDNode* build(std::vector<OurTriangle*>& triangles, int depth) const;
	bool intersect(KDNode* node, const Vec3Df& origin, const Vec3Df& dir, OurTriangle*& intersected, Vec3Df& new_origin, Vec3Df& normal, float& depth);
};

#endif /* KDNODE_H_ */
