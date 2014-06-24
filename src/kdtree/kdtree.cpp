#include "kdnode.h"
#include "kdtree.h"

KDTree::KDTree(std::vector<OurTriangle*>& triangles) {
	_root = build(triangles);
}

bool KDTree::intersect(const Vec3Df& origin, const Vec3Df& dir, Vec3Df& new_origin, Vec3Df& normal) {
	float depth = FLT_MAX;
	return _root->intersect(_root, origin, dir, _intersected, new_origin, normal, depth);
}

Vec3Df KDTree::shade(const Vec3Df& cam_pos, const Vec3Df& intersect, const Vec3Df& light_pos, const Vec3Df& normal) {
	if (this->hasMat()) {
		return _intersected->shade(cam_pos, intersect, light_pos, normal);
	} else {
		return Vec3Df(0.f,0.f,0.f);
	}
}

Vec3Df KDTree::refract(const Vec3Df &normal, const Vec3Df &dir, const float &ni, float &fresnel) {
	if (this->hasMat()) {
		return _intersected->refract(normal, dir, ni, fresnel);
	} else {
		return Vec3Df(0.f,0.f,0.f);
	}
}

KDNode* KDTree::build(std::vector<OurTriangle*>& tris) {
	KDNode* node = new KDNode();
	node->triangles = tris;
	node->left = 0;
	node->right = 0;
	node->bbox = BoundingBox();

	if (tris.size() == 0) return node;

	node->bbox = tris[0]->getBoundingBox();
	node->left = new KDNode();
	node->right = new KDNode();
	node->left->triangles = std::vector<OurTriangle*>();
	node->right->triangles = std::vector<OurTriangle*>();

	if (tris.size() == 1) return node;

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
		node->left = build(left_tris);
		node->right = build(right_tris);
	} else {
		node->left = new KDNode();
		node->right = new KDNode();
		node->left->triangles = std::vector<OurTriangle*>();
		node->right->triangles = std::vector<OurTriangle*>();
	}

	return node;
}

void KDTree::draw()
{}
