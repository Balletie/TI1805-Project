#include "boundingbox.h"

#include <algorithm>

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
