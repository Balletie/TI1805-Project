#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include "../Vec3D.h"

#include "float.h"

enum AXES {
	X,
	Y,
	Z
};

class BoundingBox {
public:
	BoundingBox(Vec3Df min = Vec3Df(FLT_MAX, FLT_MAX, FLT_MAX), Vec3Df max = Vec3Df(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	void expand(BoundingBox box);
	int longestAxis();
	bool intersect(const Vec3Df& origin, const Vec3Df& dir, float depth);
	Vec3Df _min;
	Vec3Df _max;
};

#endif /* BOUNDINGBOX_H_ */
