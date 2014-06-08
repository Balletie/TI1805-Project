#include <stdio.h>
#if (defined _WIN32 || defined _WIN64)
#include <windows.h>
#endif
#include <GL/glut.h>
<<<<<<< HEAD
#include <float.h>
#include <stdint.h>
=======
#include <cfloat>
>>>>>>> c5825a91b5904beb91912518c23cf40067015284
#include "raytracing.h"
#include "shapes.h"

//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;
std::vector<Shape*> shapes;

//use this function for any preprocessing of the mesh.
void init()
{
	//load the mesh file
	//feel free to replace cube by a path to another model
	//please realize that not all OBJ files will successfully load.
	//Nonetheless, if they come from Blender, they should.
	//MyMesh.loadMesh("/lhome/martin/Projects/Uebung/raytracing/cube.obj", true);
	MyMesh.loadMesh("cube.obj", true);
	//MyMesh.loadMesh("cube2.obj", true);
	//MyMesh.loadMesh("dodgeColorTest.obj", true);
	MyMesh.computeVertexNormals();

	//one first move: initialize the first light source
	//at least ONE light source has to be in the scene!!!
	//here, we set it to the current location of the camera
	MyLightPositions.push_back(MyCameraPosition);

	shapes.push_back(new Sphere(Vec3Df(0,0,0.2), Vec3Df(0,0,0), 1.5));
}

//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest)
{
	Vec3Df dir = origin - dest;
	dir.normalize();
	performRayTracing(origin, dir, 0, 3);
}

Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dir, uint8_t level, uint8_t max)
{
	Vec3Df normal;
	Vec3Df new_origin;
	Vec3Df color;
	float current_depth = FLT_MAX;
	bool intersection = false;

	for (unsigned int i = 0; i < shapes.size(); i++) {
		Vec3Df new_new_origin;
		Vec3Df new_normal;
		if (shapes[i]->intersect(origin, dir, new_normal, new_new_origin)) {
			intersection = true;
			float new_depth = (new_new_origin - origin).getLength();
			if (new_depth < current_depth) {
				current_depth = new_depth;
				normal = new_normal;
				new_origin = new_new_origin;
				color = shapes[i]->_color;
			}
		}
	}
	normal.normalize();
	Vec3Df reflect = dir - 2 * Vec3Df::dotProduct(dir, normal) * normal;
	if (++level == max)	return color;
	else			return color + performRayTracing(new_origin, reflect, level, max);
}

void yourDebugDraw()
{
	//draw open gl debug stuff
	//this function is called every frame

	//as an example: 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glColor3f(0,1,1);
	glBegin(GL_LINES);
	glVertex3f(testRayOrigin[0], testRayOrigin[1], testRayOrigin[2]);
	glVertex3f(testRayDestination[0], testRayDestination[1], testRayDestination[2]);
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

	std::cout<<t<<" pressed! The mouse was in location "<<x<<","<<y<<"!"<<std::endl;
}
