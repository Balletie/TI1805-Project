#include "kdnode.h"

BoundingBox::BoundingBox(Vec3Df min, Vec3Df max)
: _min(min), _max(max)
{}

void BoundingBox::expand(BoundingBox box) {
	for (int i = 0; i < 3; i++) {
		if (box._min[i] < _min[i]) _min[i] = box._min[i];
		if (box._max[i] > _max[i]) _max[i] = box._max[i];
	}

	printf("min: %f %f %f\n", _min[0], _min[1], _min[2]);
	printf("max: %f %f %f\n", _max[0], _max[1], _max[2]);
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
