#include "shapes.h"

Checkerboard::Checkerboard(Material& mat, Vec3Df org, Vec3Df coeff)
: Plane(mat, org, coeff)
{}

Vec3Df Checkerboard::shade(const Vec3Df& cam_pos, const Vec3Df& intersect, const Vec3Df& light_pos, const Vec3Df& normal) {
	int u = intersect[0]/checker_size;
	int v = intersect[2]/checker_size;
	int8_t uv_even = (u + v) % 2;
	int8_t mask_uv = uv_even >> 7;
	uint8_t abs_uv_even = (uv_even ^ mask_uv) - mask_uv;
	Vec3Df bw = abs_uv_even * Vec3Df(1.f,1.f,1.f);
	this->_mat.set_Kd(bw[0],bw[1],bw[2]);
	return Shape::shade(cam_pos, intersect, light_pos, normal);
}
