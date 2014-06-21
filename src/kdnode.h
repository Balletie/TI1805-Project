#ifndef KDNODE_H_
#define KDNODE_H_

#include <vector>

#include "shapes/shapes.h"

class BoundingBox {

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
};

#endif /* KDNODE_H_ */
