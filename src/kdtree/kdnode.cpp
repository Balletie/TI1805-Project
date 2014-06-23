#include "kdnode.h"

#include "../shapes/shapes.h"

KDNode::KDNode()
{}

KDNode::~KDNode()
{}

bool KDNode::intersect(const Vec3Df& origin, const Vec3Df& dir, OurTriangle*& intersected, Vec3Df& new_origin, Vec3Df& normal, float& depth) {
	float intersect_depth;
	if (bbox.intersect(origin,dir,intersect_depth)) {
		if (left->triangles.size() > 0 || right->triangles.size() > 0) {
			float intersect_depth_left;
			float intersect_depth_right;
			bool intersectLeft = left->bbox.intersect(origin, dir, intersect_depth_left);
			bool intersectRight = left->bbox.intersect(origin, dir, intersect_depth_right);
			KDNode* near = nullptr;
			KDNode* far = nullptr;
			if (intersect_depth_left < intersect_depth_right)	near = left, far = right;
			else							far = right, near = left;
			bool hitNear = false;
			hitNear = near->intersect(origin, dir, intersected, new_origin, normal, depth);
			if (!hitNear) {
				return far->intersect(origin, dir, intersected, new_origin, normal, depth);
			}
			return true;
			/****
			} else if (intersectLeft) {
				return left->intersect(origin, dir, intersected, new_origin, normal, depth);
			} else if (intersectRight) {
				return right->intersect(origin, dir, intersected, new_origin, normal, depth);
			} else {
				return false;
			}
			****
			bool hitLeft = left->intersect(origin, dir, intersected, new_origin, normal, depth);
			bool hitRight = right->intersect(origin, dir, intersected, new_origin, normal, depth);
			return hitLeft || hitRight;
			*/
		} else {
			bool intersection = false;
			for (int i = 0; i < triangles.size(); i++) {
				Vec3Df new_new_origin;
				Vec3Df new_normal;

				if (triangles[i]->intersect(origin, dir, new_new_origin, new_normal)) {
					intersection = true;
					float new_depth = (new_new_origin - origin).getLength();

					if (new_depth < depth) {
						depth = new_depth;
						normal = new_normal;
						new_origin = new_new_origin;
						intersected = triangles[i];
					}
				}
			}
			return intersection;
		}
	}
}
