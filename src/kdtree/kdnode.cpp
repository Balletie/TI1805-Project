#include "kdnode.h"

#include "../shapes/shapes.h"

KDNode::KDNode()
{}

KDNode::~KDNode()
{}

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
