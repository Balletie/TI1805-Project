#if (defined _WIN32 || defined _WIN64)
#include <windows.h>
#endif

#include <stdio.h>
#include <GL/glut.h>
#include "raytracing.h"


//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;

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
}

//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest)
{
	// Test intersection with circle at origin
	// Coefficients for the polynomial
	float a = Vec3Df::dotProduct(dest, dest);
	float b = 2 * Vec3Df::dotProduct(dest, origin);
	float c = Vec3Df::dotProduct(origin, origin) - 1.5 * 1.5;

	// Compute discriminant
	float disc = b * b - 4 * a * c;
	if (disc < 0) 	return Vec3Df(0,0,0);
	if (disc == 0)	return Vec3Df(0,0,1);
	if (disc > 0)	return Vec3Df(1,0,0);
	//float t0 = (-b - sqrtf(disc)) / 2 * a;
	//float t1 = (-b + sqrtf(disc)) / 2 * a;

	// Check which of the two is the right scalar to use.
	//return t0 < t1 ? camPos + t0 * ray : camPos + t1 * ray;
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
	produceRay(x, y, &testRayOrigin, &testRayDestination);

	std::cout<<t<<" pressed! The mouse was in location "<<x<<","<<y<<"!"<<std::endl;
}
