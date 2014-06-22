#include "kdnode.h"

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
