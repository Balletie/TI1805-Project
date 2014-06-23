#if (defined _WIN32 || defined _WIN64)
	#include <windows.h>
#endif

#include "shapes.h"

Shape::Shape(Material& mat, Vec3Df org)
: _mat(mat), _origin(org)
{}

Vec3Df Shape::shade(const Vec3Df& cam_pos, const Vec3Df& intersect, const Vec3Df& light_pos, const Vec3Df& normal) {
	Vec3Df ambient(0.f,0.f,0.f);
	Vec3Df diffuse(0.f,0.f,0.f);
	Vec3Df specular(0.f,0.f,0.f);

	Vec3Df light_vec = light_pos - intersect;
	light_vec.normalize();

	if (this->_mat.has_Ka()) ambient = _mat.Ka();
	if (this->_mat.has_Kd()) {
		float dot = Vec3Df::dotProduct(normal, light_vec);
		if (dot < 0)
			dot = Vec3Df::dotProduct(-normal, light_vec);
		diffuse = dot * _mat.Kd();
	}
	if(this->_mat.has_Ks()){
		Vec3Df reflect = 2 * Vec3Df::dotProduct(light_vec, normal) * normal - light_vec;
		Vec3Df view = cam_pos - intersect;
		view.normalize();

		if (Vec3Df::dotProduct(view, reflect) < 0) {
			specular = Vec3Df(0, 0, 0);
		} else {
			float shininess;
			if (this->_mat.has_Ns()) shininess = _mat.Ns();
			else shininess = 21;
			specular = pow(Vec3Df::dotProduct(reflect, view), shininess) * _mat.Ks();
		}
	}
	return ambient + diffuse + specular;
}

Vec3Df Shape::refract(const Vec3Df &normal, const Vec3Df &dir, const float &ni, float &fresnel) {
	if (this->_mat.has_Ni()) {
		double dot = Vec3Df::dotProduct(normal, dir);
		double ni1, ni2;
		Vec3Df realNormal = normal;

		// If dot(N,D) > 0, then we're exiting the medium
		if (dot > 0) {
			ni1 = this->_mat.Ni();
			ni2 = ni;
			realNormal = -normal;

			// While we're at it, calculate Fresnel with Schlick's approximation.
			// Do nothing in the else statement, as Fresnel is initialised to zero
			// in the calling function.
			float fzero = pow(((ni1 - ni2) / (ni1 + ni2)), 2);
			fresnel = (fzero + (1 - fzero) * pow((1 - dot), 5)) / 100;
		} else {
			// FIXME: fresnel here too?
			ni1 = ni;
			ni2 = this->_mat.Ni();
			dot = Vec3Df::dotProduct(-normal, dir);
			

			float fzero = pow(((ni1 - ni2) / (ni1 + ni2)), 2);
			fresnel = (fzero + (1 - fzero) * pow((1 - dot), 5)) / 100;
		}

		// If root < 0, total internal reflection takes place.
		// In this case, the refraction vector should be black.
		// Maybe we could integrate reflection here
		double n = ni1 / ni2;
		double root = 1.0 - n * n * (1.0 - dot * dot);

		if (root < 0) return dir - 2 * dot * realNormal;

		return dir * n + (n * dot - sqrt(root)) * realNormal;
	}

	return Vec3Df(0,0,0);
}
