#include "shapes.h"

Checkerboard::Checkerboard(Material& mat, Vec3Df org, Vec3Df coeff)
: Plane(mat, org, coeff)
{}

Vec3Df Checkerboard::shade(const Vec3Df& cam_pos, const Vec3Df& intersect, const Vec3Df& light_pos, const Vec3Df& normal) {
	int u = intersect[0]/checker_size;
	int v = intersect[1]/checker_size;
	int8_t u_even = u % 2;
	int8_t v_even = v % 2;
	int8_t mask_u = u_even >> 7;
	int8_t mask_v = v_even >> 7;
	uint8_t abs_u_even = (u_even ^ mask_u) - mask_u;
	uint8_t abs_v_even = (v_even ^ mask_v) - mask_v;
	float xor_bw = (float) (abs_u_even^abs_v_even);
	Vec3Df bw = xor_bw * Vec3Df(1.f,1.f,1.f);
	this->_mat.set_Kd(bw[0],bw[1],bw[2]);
	return Shape::shade(cam_pos, intersect, light_pos, normal);
}
