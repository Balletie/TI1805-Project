#include <stdio.h>
#if (defined _WIN32 || defined _WIN64)
#include <windows.h>
#endif
#include <GL/glut.h>
#include <float.h>
#include <stdint.h>
#include "raytracing.h"
#include "kdtree/kdtree.h"
#include "shapes/shapes.h"

//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;
Vec3Df new_orig;
Vec3Df new_dest;
std::vector<OurObject*> shapes;
std::vector<Material> materials;
std::map<std::string, Texture*> textures;
std::vector<OurTriangle*> triangles;

//use this function for any preprocessing of the mesh.
void init()
{
	//load the mesh file
	//feel free to replace cube by a path to another model
	//please realize that not all OBJ files will successfully load.
	//Nonetheless, if they come from Blender, they should.
	//MyMesh.loadMesh("meshes/Pen_low.obj", true);
	//MyMesh.loadMesh("meshes/final.obj", true);
	//MyMesh.loadMesh("meshes/Pen_low.obj", true);
	//MyMesh.computeVertexNormals();

	//one first move: initialize the first light source
	//at least ONE light source has to be in the scene!!!
	//here, we set it to the current location of the camera
	MyLightPositions.push_back(MyCameraPosition + Vec3Df(0, 4, 0));
	MyLightPositions.push_back(MyCameraPosition + Vec3Df(2, 4, 0));

	Material plane_mat;
	//plane_mat.set_Ka(0.2,0.2,0.2);
	plane_mat.set_Kd(0.2,0.2,0.2);
	plane_mat.set_Ks(0.5,0.5,0.5);
	//plane_mat.set_Ni(1.7); //glass refractive index;
	plane_mat.set_Tr(1.0);
	materials.push_back(plane_mat);

	Material earthmat;
	earthmat.set_Kd(0.2,0.f,0.f);
	earthmat.set_Ks(0.2,0.2,0.2);
	earthmat.set_Ni(1.3);
	earthmat.set_Tr(0.5);
	earthmat.set_textureName("meshes/textures/earthmap1k.ppm");
	Image earth_img("meshes/textures/earthmap1k.ppm");
	Texture* earth_tex = new Texture(earth_img);
	materials.push_back(earthmat);

	Material blue;
	blue.set_Kd(0  , 0  , 0.2);
	blue.set_Ks(0.2, 0.2, 0.2);
	blue.set_Ni(1.3330); //Water at 20 degrees C
	blue.set_Tr(0.5);
	blue.set_textureName("meshes/textures/sisal.ppm");
	Image sisal_img("meshes/textures/sisal.ppm");
	Texture* sisal_tex = new Texture(sisal_img);
	materials.push_back(blue);
	
	Material brown_ish;
	brown_ish.set_Kd(0.4, 0.4, 0  );
	brown_ish.set_Ks(0.2, 0.2, 0.2);
	brown_ish.set_Ni(3.0);
	brown_ish.set_Tr(1);
	brown_ish.set_textureName("meshes/textures/Sky.ppm");
	Image Sky_img("meshes/textures/Sky.ppm");
	Texture* Sky_tex = new Texture(Sky_img);
	materials.push_back(brown_ish);

	Material grey;
	//grey.set_Kd(0.1, 0.1, 0.1);
	//grey.set_Ks(1  , 1  , 1  );
	grey.set_Ni(1.3);
	grey.set_Tr(0.3);
	materials.push_back(grey);

	Material red;
	red.set_Kd(0.2,0.f,0.f);
	red.set_Ks(0.2,0.2,0.2);
	red.set_Ni(1.3);
	red.set_Tr(0.5);
	materials.push_back(red);

	OurObject* earth = new Sphere(materials[1], Vec3Df(0, 1, 4), 1);
	earth->setTexture(earth_tex);
	shapes.push_back(earth);
	OurObject* sisal = new Sphere(materials[2], Vec3Df(-2, 1, 4), 1);
	sisal->setTexture(sisal_tex);
	shapes.push_back(sisal);
	OurObject* Sky = new Sphere(materials[3], Vec3Df(0, 1, 2), 1);
	Sky->setTexture(Sky_tex);
	shapes.push_back(Sky);
	shapes.push_back(new Sphere(materials[4], Vec3Df(0, 1, 6), 1));

	// Plane(color, origin, coeff)
	// Horizontal green plane
	//shapes.push_back(new Plane(materials[0], Vec3Df(0,-1,0), Vec3Df(0,1,0)));
	// Vertical red plane
	//shapes.push_back(new Plane(materials[5], Vec3Df(0,0,-4), Vec3Df(0,0,1)));
	//shapes.push_back(new Plane(materials[5], Vec3Df(0,-4,0), Vec3Df(1,0,0)));
	// Checkerboard
	shapes.push_back(new Checkerboard(materials[0], Vec3Df(0,-1,0), Vec3Df(0,1,0)));

	//Image img("meshes/textures/wood_norm3.ppm");
	//Texture* test = new Texture(img);
	/*std::vector<Triangle>::iterator iter = MyMesh.triangles.begin();
	for (int i = 0; i < MyMesh.triangles.size(); i++) {
		Material& mat = MyMesh.materials[MyMesh.triangleMaterials[i]];
		OurTriangle* t = new OurTriangle(mat,&MyMesh,&*(iter + i));
		if (mat.has_tex()) {
			t->setTexture(textures.at(mat.textureName()));
			//t->setNormalMap(test);
		}
		triangles.push_back(t);
	}*/

	//shapes.push_back(new KDTree(triangles));
}

//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest)
{
	// Get the direction of the vector.
	Vec3Df dir = dest - origin;
	dir.normalize();
	return performRayTracing(origin, dir, 0, 6);
}

Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dir, uint8_t level, uint8_t max)
{
	if (level == max) return Vec3Df(0,0,0);

	// This will be instantiated with the new normal at the intersection point.
	Vec3Df normal;
	// This will be instantiated with the coordinates of the intersection point.
	Vec3Df new_origin;
	//Reference to the intersected object.
	Shape* intersected = nullptr;

	float current_depth = FLT_MAX;
	bool intersection = false;
	for (int i = 0; i < shapes.size(); i++) {
		Vec3Df new_new_origin;
		Vec3Df new_normal;
		if (shapes[i]->intersect(origin, dir, new_new_origin, new_normal)) {
			float new_depth = (new_new_origin - origin).getLength();
			if (new_depth < current_depth) {
				intersection = true;
				current_depth = new_depth;
				normal = new_normal;
				new_origin = new_new_origin;
				intersected = shapes[i]->getIntersected();
			}
		}
	}
	// There was no intersection, so return background color.
	if (!intersection) return Vec3Df(0.f, 0.f, 0.f);

	Vec3Df refractedColor = Vec3Df(0.f, 0.f, 0.f);
	Vec3Df reflectedColor = Vec3Df(0.f, 0.f, 0.f);
	double dotProduct = Vec3Df::dotProduct(dir, normal);
	double reflectivity = 1.0f, transmission = 0.f; //FIXME: initial values correct?

	if (intersected->hasMat()) {
		//refraction
		if (intersected->getMat().has_Ni()) {
			float ni_air = 1.0f;
			float fresnel = 0.f;

			Vec3Df refract = intersected->refract(normal, dir, ni_air, fresnel);

			reflectivity = fresnel;
			transmission = 1 - fresnel;

			float translucency = 0.f;
			if (intersected->getMat().has_Tr()) {
				translucency = 1 - intersected->getMat().Tr();
				if (translucency > 0)
					refractedColor = translucency * performRayTracing(new_origin + refract * EPSILON, refract, level + 1, max);
			}
		}
		//refection
		if (intersected->getMat().has_Ks()) {
			Vec3Df reflect = dir - 2 * dotProduct * normal;
			if (reflectivity > 0)
				reflectedColor = performRayTracing(new_origin, reflect, level + 1, max);
		}

	}

	// The color of the intersected object for all lightsources.
	Vec3Df directColor = Vec3Df(0.f, 0.f, 0.f);
	Shape* shadowInt = nullptr;

	// Calculate shadows. multiple light sources. transparant shadows.
	for (unsigned int j = 0; j < MyLightPositions.size(); j++) {
		Vec3Df lightDir = MyLightPositions[j] - new_origin;
		float lightDist = lightDir.getLength();
		bool intersection = false;

		for (unsigned int i = 0; i < shapes.size(); i++) {
			Vec3Df hit, stub2;
			// Check whether there's an intersection between the hit point and the light source
			if (shapes[i]->intersect(new_origin, lightDir, hit, stub2) && (hit - new_origin).getLength() < lightDist) {
				intersection = true;
				shadowInt = shapes[i]->getIntersected();

				if (!shadowInt->_mat.has_Tr() || shadowInt->_mat.Tr() == 1.0) {
					// Intersected with an opaque object.
					break;
				} else {
					// Material is transparent
					directColor +=  (1 - shapes[i]->getMat().Tr()) * intersected->shade(origin, new_origin, MyLightPositions[j], normal); 
					// If it has an ambient color, it should let that color pass through.
					if (shapes[i]->getMat().has_Ka() && shapes[i]->getMat().Ka() != Vec3Df(0.f, 0.f, 0.f)) {
						directColor *= shapes[i]->getMat().Ka();
					}
				}
			}
		}
		if (!intersection) {
			// There was no intersection.
			directColor += intersected->shade(origin, new_origin, MyLightPositions[j], normal);
		}
	}
	directColor /= MyLightPositions.size();

	return directColor + reflectivity * reflectedColor + transmission * refractedColor;
}

void yourDebugDraw()
{
	
	//draw open gl debug stuff
	//this function is called every frame

	//as an example: 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);

	// Draw all the shapes for the viewport window.
	for (int i = 0; i < shapes.size(); i++) {
		shapes[i]->draw();
	}

	glColor3f(0,1,1);
	glBegin(GL_LINES);
		glVertex3f(testRayOrigin[0], testRayOrigin[1], testRayOrigin[2]);
		glVertex3f(testRayDestination[0], testRayDestination[1], testRayDestination[2]);
		glVertex3f(new_orig[0], new_orig[1], new_orig[2]);
		glVertex3f(new_dest[0], new_dest[1], new_dest[2]);
	glEnd();

	glPointSize(10);
	glBegin(GL_POINTS);
		glVertex3fv(MyLightPositions[0].pointer());
	glEnd();

	glPopAttrib();
}

void yourKeyboardFunc(char t, int x, int y)
{
	// do what you want with the keyboard input t.
	// x, y are the screen position
	std::cout<< t <<" pressed! The mouse was in location "<<x<<","<<y<<"!"<<std::endl;
}
