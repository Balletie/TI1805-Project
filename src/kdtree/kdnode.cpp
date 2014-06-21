#include "kdnode.h"

BoundingBox::BoundingBox(Vec3Df min = Vec3Df(0,0,0), Vec3Df max = Vec3Df(0,0,0))
: _min(min), _max(max)
{
	printf("min: %f %f %f\n", min[0], min[1], min[2]);
	printf("max: %f %f %f\n", max[0], max[1], max[2]);
}


KDNode::KDNode() {
	// TODO Auto-generated constructor stub

}

KDNode::~KDNode() {
	// TODO Auto-generated destructor stub
}

KDNode* KDNode::build(std::vector<OurTriangle*>& tris, int depth) const {
	KDNode* node = new KDNode();
	node->triangles = tris;
	node->left = 0;
	node->right = 0;
	node->bbox = BoundingBox();

}
