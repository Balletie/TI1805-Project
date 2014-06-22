#include <stdio.h>
#if (defined _WIN32 || defined _WIN64)
#include <windows.h>
#endif
#include <GL/glut.h>
#include <float.h>
#include <stdint.h>

#include "raytracing.h"

#include "kdtree/kdnode.h"
#include "shapes/shapes.h"

//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;
Vec3Df new_orig;
Vec3Df new_dest;
std::vector<Shape*> shapes;
std::vector<Material> materials;
std::vector<OurTriangle*> triangles;
KDNode* node = new KDNode();

//use this function for any preprocessing of the mesh.
void init()
{
	//load the mesh file
	//feel free to replace cube by a path to another model
	//please realize that not all OBJ files will successfully load.
	//Nonetheless, if they come from Blender, they should.
	MyMesh.loadMesh("meshes/cube.obj", true);
	//MyMesh.loadMesh("meshes/Pen_subsurf.obj", true);
	MyMesh.computeVertexNormals();

	//one first move: initialize the first light source
	//at least ONE light source has to be in the scene!!!
	//here, we set it to the current location of the camera
	MyLightPositions.push_back(MyCameraPosition + Vec3Df(0, 4, 0));

	Material plane_mat;
	//plane_mat.set_Ka(0.2,0.2,0.2);
	plane_mat.set_Kd(0.2,0.2,0.2);
	plane_mat.set_Ks(0.5,0.5,0.5);
	materials.push_back(plane_mat);

	Material red;
	red.set_Kd(0.2,0.f,0.f);
	red.set_Ks(0.2,0.2,0.2);
	materials.push_back(red);

	Material blue;
	blue.set_Kd(0  , 0  , 0.2);
	blue.set_Ks(0.2, 0.2, 0.2);
	materials.push_back(blue);
	
	Material brown_ish;
	brown_ish.set_Kd(0.4, 0.4, 0  );
	brown_ish.set_Ks(0.2, 0.2, 0.2);
	materials.push_back(brown_ish);

	Material grey;
	grey.set_Kd(0.1, 0.1, 0.1);
	grey.set_Ks(1  , 1  , 1  );
	materials.push_back(grey);

	//shapes.push_back(new Sphere(materials[1], Vec3Df(-2, 0, -1), 1));
	//shapes.push_back(new Sphere(materials[2], Vec3Df( 0, 0, -1), 1));
	//shapes.push_back(new Sphere(materials[3], Vec3Df( 0, 2, -1), 1));
	//shapes.push_back(new Sphere(materials[4], Vec3Df( 0, 0, -1), 1));

	// Plane(color, origin, coeff)
	// Horizontal green plane
	//shapes.push_back(new Plane(materials[0], Vec3Df(0,-1,0), Vec3Df(0,1,0)));
	// Vertical red plane
	//shapes.push_back(new Plane(Vec3Df(0.2,0,0), Vec3Df(0,0,0), Vec3Df(0,0,1)));
	// Checkerboard
	//shapes.push_back(new Checkerboard(materials[0], Vec3Df(0,-1,0), Vec3Df(0,1,0)));

	std::vector<Triangle>::iterator iter = MyMesh.triangles.begin();
	for (int i = 0; i < MyMesh.triangles.size(); i++) {
		triangles.push_back(new OurTriangle(MyMesh.materials[MyMesh.triangleMaterials[i]], &MyMesh, &*(iter + i)));
	}

	node = node->build(triangles, 10);

	printf("root: %d\n", node->triangles.size());
	printf("min: %f %f %f\n", node->bbox._min[0], node->bbox._min[1], node->bbox._min[2]);
	printf("max: %f %f %f\n", node->bbox._max[0], node->bbox._max[1], node->bbox._max[2]);
	printf("root->left: %d\n", node->left->triangles.size());
	printf("root->left->min: %f %f %f\n", node->left->bbox._min[0], node->left->bbox._min[1], node->left->bbox._min[2]);
	printf("root->left->max: %f %f %f\n", node->left->bbox._max[0], node->left->bbox._max[1], node->left->bbox._max[2]);
	Vec3Df x = MyMesh.vertices[node->left->triangles[0]->_triangle->v[0]].p;
	Vec3Df y = MyMesh.vertices[node->left->triangles[0]->_triangle->v[1]].p;
	Vec3Df z = MyMesh.vertices[node->left->triangles[0]->_triangle->v[2]].p;
	printf("root->left->triangle[0].x: %f %f %f\n", x[0], x[1], x[2]);
	printf("root->left->triangle[0].y: %f %f %f\n", y[0], y[1], y[2]);
	printf("root->left->triangle[0].z: %f %f %f\n", z[0], z[1], z[2]);
	x = MyMesh.vertices[node->left->triangles[1]->_triangle->v[0]].p;
	y = MyMesh.vertices[node->left->triangles[1]->_triangle->v[1]].p;
	z = MyMesh.vertices[node->left->triangles[1]->_triangle->v[2]].p;
	printf("root->left->triangle[1].x: %f %f %f\n", x[0], x[1], x[2]);
	printf("root->left->triangle[1].y: %f %f %f\n", y[0], y[1], y[2]);
	printf("root->left->triangle[1].z: %f %f %f\n", z[0], z[1], z[2]);
	printf("root->right: %d\n", node->right->triangles.size());
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
	OurTriangle* intersected;

	float current_depth = FLT_MAX;
	if(!node->intersect(node, origin, dir, intersected, new_origin, normal, current_depth)) return Vec3Df(0.f,0.f,0.f);

	// Re-use intersection here
	/*bool intersection = false;
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
	if (intersection)	return Vec3Df(0.f,0.f,0.f);*/

	normal.normalize();
	// Compute the reflection vector for the next recursive call.
	Vec3Df reflect = dir - 2 * Vec3Df::dotProduct(dir, normal) * normal;

	// Shade the object.
	color = intersected->shade(origin, new_origin, MyLightPositions[0], normal);
	reflectivity = intersected->_mat.Ks();

	if (++level == max)	return color;
	else return color + reflectivity * performRayTracing(new_origin, reflect, level, max);
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
	produceRay(x, y, testRayOrigin, testRayDestination);

	Vec3Df orig = testRayOrigin;
	Vec3Df dir = testRayDestination - testRayOrigin;
	dir.normalize();

	OurTriangle *intersected;
	Vec3Df new_orig;
	Vec3Df normal;
	float depth = FLT_MAX;
	node->intersect(node, orig, dir, intersected, new_orig, normal, depth);
	printf("\n\n");
	//node->left->bbox.intersect(orig, dir);
}
