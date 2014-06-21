#if (defined _WIN32 || defined _WIN64)
	#include <windows.h>
#endif

#include "shapes.h"

Shape::Shape(Material& mat, Vec3Df org)
: _mat(mat), _origin(org)
{}

Vec3Df Shape::shade(const Vec3Df& cam_pos, const Vec3Df& intersect, const Vec3Df& light_pos, const Vec3Df& normal)  {
	Vec3Df ambient(0.f,0.f,0.f);
	Vec3Df diffuse(0.f,0.f,0.f);
	Vec3Df specular(0.f,0.f,0.f);

	Vec3Df light_vec = light_pos - intersect;
	light_vec.normalize();

	if (this->_mat.has_Ka()) ambient = _mat.Ka();
	if (this->_mat.has_Kd()) {
		diffuse = Vec3Df::dotProduct(normal, light_vec) * _mat.Kd();
	}
	if(this->_mat.has_Ks()){
		Vec3Df reflect = 2 * Vec3Df::dotProduct(light_vec, normal) * normal - light_vec;
		Vec3Df view = cam_pos - intersect;
		view.normalize();

		if (Vec3Df::dotProduct(view, reflect) < 0) {
			specular = Vec3Df(0, 0, 0);
		} else {
			float shininess;
			if (this->_mat.has_Ns()) 	shininess = _mat.Ns();
			else				shininess = 21;
			specular = pow(Vec3Df::dotProduct(reflect, view), shininess) * _mat.Ks();
		}
	}
	return ambient + diffuse + specular;
}
