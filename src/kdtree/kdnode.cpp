#include "kdnode.h"

#include "../shapes/shapes.h"

BoundingBox::BoundingBox(Vec3Df min, Vec3Df max)
: _min(min), _max(max)
{}

void BoundingBox::expand(BoundingBox box) {
	for (int i = 0; i < 3; i++) {
		if (box._min[i] < _min[i]) _min[i] = box._min[i];
		if (box._max[i] > _max[i]) _max[i] = box._max[i];
	}
}

int BoundingBox::longestAxis() {
	int lengthX = this->_max[0] - this->_min[0];
	int lengthY = this->_max[1] - this->_min[1];
	int lengthZ = this->_max[2] - this->_min[2];

	if (lengthX > lengthY && lengthX > lengthZ)
		return X;
	if (lengthY > lengthX && lengthY > lengthZ)
		return Y;
	return Z;
}

bool BoundingBox::intersect(const Vec3Df& origin, const Vec3Df& dir) {
	float denomx = 1.0f / dir[0];
	float denomy = 1.0f / dir[1];
	float denomz = 1.0f / dir[2];

	float txmin = (_min[0] - origin[0]) * denomx;
	float tymin = (_min[1] - origin[1]) * denomy;
	float tzmin = (_min[2] - origin[2]) * denomz;

	float txmax = (_max[0] - origin[0]) * denomx;
	float tymax = (_max[1] - origin[1]) * denomy;
	float tzmax = (_max[2] - origin[2]) * denomz;

	float tmin = std::max(std::max(std::min(txmin, txmax), std::min(tymin, tymax)), std::min(tzmin, tzmax));
	float tmax = std::min(std::min(std::max(txmin, txmax), std::max(tymin, tymax)), std::max(tzmin, tzmax));

	if (tmax < 0) return false;
	if (tmin > tmax) return false;

	return true;
}

KDNode::KDNode()
{}

KDNode::~KDNode()
{}

KDNode* KDNode::build(std::vector<OurTriangle*>& tris, int depth) const {
	KDNode* node = new KDNode();
	node->triangles = tris;
	node->left = 0;
	node->right = 0;
	node->bbox = BoundingBox();

	if (tris.size() == 0)
		return node;

	node->bbox = tris[0]->getBoundingBox();
	node->left = new KDNode();
	node->right = new KDNode();
	node->left->triangles = std::vector<OurTriangle*>();
	node->right->triangles = std::vector<OurTriangle*>();

	if (tris.size() == 1)
		return node;

	for (int i = 1; i < tris.size(); i++) {
		node->bbox.expand(tris[i]->getBoundingBox());
	}

	Vec3Df midpt = Vec3Df(0,0,0);
	for (int i = 0; i < tris.size(); i++) {
		midpt = midpt + (tris[i]->getMidPoint() * (1.0 / tris.size()));
	}

	std::vector<OurTriangle*> left_tris;
	std::vector<OurTriangle*> right_tris;
	int axis = node->bbox.longestAxis();
	for (int i = 0; i < tris.size(); i++) {
		switch (axis) {
		case X:
			midpt[0] >= tris[i]->getMidPoint()[0] ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		case Y:
			midpt[1] >= tris[i]->getMidPoint()[1] ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		case Z:
			midpt[2] >= tris[i]->getMidPoint()[2] ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		}
	}

	if (left_tris.size() == 0 && right_tris.size() > 0) left_tris = right_tris;
	if (right_tris.size() == 0 && left_tris.size() > 0) right_tris = left_tris;

	int matches = 0;
	for (int i = 0; i < left_tris.size(); i++) {
		for (int j = 0; j < right_tris.size(); j++) {
			if (left_tris[i] == right_tris[j])
				matches++;
		}
	}

	if ((float)matches / left_tris.size() < 0.5 && (float)matches / right_tris.size() < 0.5) {
		node->left = build(left_tris, ++depth);
		node->right = build(right_tris, ++depth);
	} else {
		node->left = new KDNode();
		node->right = new KDNode();
		node->left->triangles = std::vector<OurTriangle*>();
		node->right->triangles = std::vector<OurTriangle*>();
	}

	return node;
}

bool KDNode::intersect(KDNode* node, const Vec3Df& origin, const Vec3Df& dir, OurTriangle*& intersected, Vec3Df& new_origin, Vec3Df& normal, float& depth) {
	if (node->bbox.intersect(origin, dir)) {
		if (node->left->triangles.size() > 0 || node->right->triangles.size() > 0) {
			bool hitLeft = intersect(node->left, origin, dir, intersected, new_origin, normal, depth);
			bool hitRight = intersect(node->right, origin, dir, intersected, new_origin, normal, depth);

			return hitLeft || hitRight;
		} else {
			bool intersection = false;
			for (int i = 0; i < node->triangles.size(); i++) {
				Vec3Df new_new_origin;
				Vec3Df new_normal;

				if (node->triangles[i]->intersect(origin, dir, new_new_origin, new_normal)) {
					intersection = true;
					float new_depth = (new_new_origin - origin).getLength();

					if (new_depth < depth) {
						depth = new_depth;
						normal = new_normal;
						new_origin = new_new_origin;
						intersected = node->triangles[i];
					}
				}
			}
			return intersection;
		}
	}

	return false;
}