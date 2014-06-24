#ifndef KDTREE_H
#define KDTREE_H
#include "../shapes/shapes.h"
#include "../Vec3D.h"

class KDNode;
class KDTree : public OurObject {
	public:
		KDTree(std::vector<OurTriangle*>& triangles);
		virtual bool intersect(const Vec3Df&, const Vec3Df&, Vec3Df&, Vec3Df&);
		virtual Vec3Df shade(const Vec3Df&, const Vec3Df&, const Vec3Df&, const Vec3Df&);
		virtual Vec3Df refract(const Vec3Df&, const Vec3Df&, const float&, float&);
		virtual void draw();
		virtual bool hasMat() { return _intersected != nullptr; };
		virtual Material& getMat() { return _intersected->_mat; };
		static KDNode* build(std::vector<OurTriangle*>& triangles);
	private:
		KDNode* _root;
		OurTriangle* _intersected;
};

#endif
