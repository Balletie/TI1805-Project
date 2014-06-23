#ifndef KDNODE_H_
#define KDNODE_H_

#include <vector>
#include "float.h"

#include "../Vec3D.h"

#include "../shapes/boundingbox.h"

// FORWARD DECLARATION
class OurTriangle;
class Shape;

class KDNode {
public:
	BoundingBox bbox;
	KDNode* left;
	KDNode* right;
	std::vector<OurTriangle*> triangles;

	KDNode();
	virtual ~KDNode();

	bool intersect(const Vec3Df& origin, const Vec3Df& dir, OurTriangle*& intersected, Vec3Df& new_origin, Vec3Df& normal, float& depth);
};

#endif /* KDNODE_H_ */
