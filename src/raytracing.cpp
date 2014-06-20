#include <stdio.h>
#if (defined _WIN32 || defined _WIN64)
#include <windows.h>
#endif
#include <GL/glut.h>
#include <float.h>
#include <stdint.h>
#include "raytracing.h"
#include "shapes.h"

//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;
Vec3Df new_orig;
Vec3Df new_dest;
std::vector<Shape*> shapes;
std::vector<Material> materials;

//use this function for any preprocessing of the mesh.
void init()
{
	//load the mesh file
	//feel free to replace cube by a path to another model
	//please realize that not all OBJ files will successfully load.
	//Nonetheless, if they come from Blender, they should.
	//MyMesh.loadMesh("cube.obj", true);
	//MyMesh.loadMesh("Pen_low.obj", true);
	//MyMesh.computeVertexNormals();

	//one first move: initialize the first light source
	//at least ONE light source has to be in the scene!!!
	//here, we set it to the current location of the camera
	MyLightPositions.push_back(MyCameraPosition + Vec3Df(0, 4, 0));

	Material plane_mat;
	plane_mat.set_Ka(0.2,0.2,0.2);
	plane_mat.set_Kd(0.2,0.2,0.2);
	plane_mat.set_Ks(0.5,0.5,0.5);
	plane_mat.set_Ni(1.7); //glass refractive index;
	materials.push_back(plane_mat);

	Material red;
	red.set_Kd(0.2,0.f,0.f);
	red.set_Ks(0.2,0.2,0.2);
	red.set_Ni(1.7); //glass refractive index;
	materials.push_back(red);

	Material blue;
	blue.set_Kd(0  , 0  , 0.2);
	blue.set_Ks(0.2, 0.2, 0.2);
	blue.set_Ni(1.7); //glass refractive index;
	materials.push_back(blue);
	
	Material brown_ish;
	brown_ish.set_Kd(0.4, 0.4, 0  );
	brown_ish.set_Ks(0.2, 0.2, 0.2);
	brown_ish.set_Ni(1.7); //glass refractive index;
	materials.push_back(brown_ish);

	Material grey;
	grey.set_Kd(0.1, 0.1, 0.1);
	grey.set_Ks(1  , 1  , 1  );
	grey.set_Ni(1.7); //glass refractive index;
	materials.push_back(grey);

	shapes.push_back(new Sphere(materials[1], Vec3Df(-2, 0, -1), 1));
	shapes.push_back(new Sphere(materials[2], Vec3Df( 0, 0, -1), 1));
	shapes.push_back(new Sphere(materials[3], Vec3Df( 0, 2, -1), 1));
	shapes.push_back(new Sphere(materials[4], Vec3Df( 2, 0, -1), 1));

	// Plane(color, origin, coeff)
	// Horizontal green plane
	//shapes.push_back(new Plane(materials[0], Vec3Df(0,-1,0), Vec3Df(0,1,0)));
	// Vertical red plane
	//shapes.push_back(new Plane(Vec3Df(0.2,0,0), Vec3Df(0,0,0), Vec3Df(0,0,1)));

	//std::vector<Triangle>::iterator iter;
    //int i;
	//for (iter = MyMesh.triangles.begin(), i = 0; iter != MyMesh.triangles.end(), i < MyMesh.triangles.size(); ++iter, i++) {
    //    shapes.push_back(new OurTriangle(MyMesh.materials[MyMesh.triangleMaterials[i]], &MyMesh, &(*iter)));
	//}
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
	// This will be instantiated with the new normal at the intersection point.
	Vec3Df normal;
	// This will be instantiated with the coordinates of the intersection point.
	Vec3Df new_origin;

	// The color if the intersected object.
	Vec3Df color;
	// The reflectivity of the intersected object.
	Vec3Df reflectivity;

	//Reference to the intersected object.
	Shape *intersected;

	float current_depth = FLT_MAX;
	bool intersection = false;

	// Naive: trace each object.
	for (unsigned int i = 0; i < shapes.size(); i++) {
		Vec3Df new_new_origin;
		Vec3Df new_normal;
		if (shapes[i]->intersect(origin, dir, new_new_origin, new_normal)) {
			intersection = true;
			float new_depth = (new_new_origin - origin).getLength();
			if (new_depth < current_depth) {
				current_depth = new_depth;
				normal = new_normal;
				new_origin = new_new_origin;
				intersected = shapes[i];
			}
		}
	}
	if (!intersection) return Vec3Df(0.f, 0.f, 0.f);

	// Re-use intersection here
	intersection = false;
	for (unsigned int i = 0; i < shapes.size(); i++) {
		Vec3Df stub1;
		Vec3Df stub2;
		Vec3Df lightPos = MyLightPositions[0] - new_origin;
		float lightDist = lightPos.getLength();

		if (shapes[i]->intersect(new_origin, lightPos, stub1, stub2)) {
			if ((stub1 - new_origin).getLength() < lightDist)
				intersection = true;
		}
	}
	// If there was an intersection, this spot is occluded.
	if (intersection) return Vec3Df(0.f, 0.f, 0.f);

	normal.normalize();
	// Compute the reflection vector for the next recursive call.
	Vec3Df reflect = dir - 2 * Vec3Df::dotProduct(dir, normal) * normal;

	// Shade the object.
	color = intersected->shade(origin, new_origin, MyLightPositions[0], normal);
	reflectivity = intersected->_mat.Ks();

	// Calculate refractions. For simplicity, all vectors must be normalized.
	// It is assumed we are either inside an object or in air.
	Vec3Df refract = Vec3Df(0.f, 0.f, 0.f);
	bool has_Ni = intersected->_mat.has_Ni();
	if (has_Ni) {
		double ratio = 0.f;
		double root = 0.f;
		double ni_air = 1.0;
		double ni_mat = intersected->_mat.Ni();
		double dotProduct = Vec3Df::dotProduct(dir, normal);
		if (dotProduct > 0) {
			// The normal and dir are in the same direction, meaning we are
			// calculating the outgoing vector.
			ratio = ni_mat / ni_air;
			root = 1 - ni_mat * ni_mat * (1 - dotProduct * dotProduct) / (ni_air * ni_air);
		} else {
			// The normal and dir are in the opposite direction, meaning we
			// are calculating the vector inside the object.
			ratio = ni_air / ni_mat;
			root = 1 - ni_air * ni_air * (1 - dotProduct * dotProduct) / (ni_mat * ni_mat);
		}
		// If root < 0, total internal reflection takes place. In this case,
		// the refraction vector should be black. It already is, so do nothing here.
		if (root >= 0) {
			refract = ratio * (dir - dotProduct * normal) - normal * sqrt(root);
		}
	}

	if (++level == max) {
		return color;
	} else if (refract.getLength() != 0) {
		return color + reflectivity * performRayTracing(new_origin, reflect, level, max)
			+ performRayTracing(dir, refract, level, max);
	} else {
		return color + reflectivity * performRayTracing(new_origin, reflect, level, max);
	}
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

	//here I use it to get the coordinates of a ray, which I then draw in the debug function.
	produceRay(x, y, testRayOrigin, testRayDestination);

	Vec3Df orig = testRayOrigin;
	Vec3Df dir = testRayDestination - testRayOrigin;
	dir.normalize();

	Vec3Df normal;
	if (shapes[0]->intersect(orig, dir, new_orig, normal)) {
		printf("intersection!!\n");
		Vec3Df reflect = dir - 2 * Vec3Df::dotProduct(dir, normal) * normal;
		new_dest = 20 * reflect;
	}

	std::cout<< t <<" pressed! The mouse was in location "<<x<<","<<y<<"!"<<std::endl;

	/*
	Triangle triangle;
	Vertex vertex;
	for (int i = 0; i < MyMesh.triangles.size(); i++) {
		triangle = MyMesh.triangles[i];
		printf("triangle index v: %d\t %d\t %d\n", triangle.v[0], triangle.v[1], triangle.v[2]);
		printf("triangle index t: %d\t %d\t %d\n", triangle.t[0], triangle.t[1], triangle.t[2]);
		for (int j = 0; j < 3; j++) {
			vertex = MyMesh.vertices[triangle.v[j]];
			printf("triangle coords: %f\t %f\t %f\n", vertex.p[0], vertex.p[1], vertex.p[2]);
		}
	}
	*/
}
