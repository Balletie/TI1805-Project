#ifndef MESH_H_sdfjlasdfjfsdfjljfasdf
#define MESH_H_sdfjlasdfjfsdfjljfasdf

#include "Vertex.h"
#include <vector>
#include <map>
#include <string>


class Material {
	public:

	Material() { cleanup(); };

	Material(const Material & m) 
	{ 
		*this=m;
	};

	Material& operator=(const Material& m)
	{ 
		Kd_=m.Kd_;		 
		Kd_is_set_=m.Kd_is_set_; // diffuse
		Ka_=m.Ka_;		 
		Ka_is_set_=m.Ka_is_set_; // ambient
		Ks_=m.Ks_;		 
		Ks_is_set_=m.Ks_is_set_; // specular
		
		Ns_=m.Ns_;		 
		Ns_is_set_=m.Ns_is_set_; // specular
		Ni_=m.Ni_;		 
		Ni_is_set_=m.Ni_is_set_; // specular

		Tr_=m.Tr_;
		Tr_is_set_=m.Tr_is_set_; // transperency
		illum_ = m.illum_;
		name_=m.name_;
		return (*this);
	};

	void cleanup()
	{
		Kd_is_set_ = false;
		Ka_is_set_ = false;
		Ks_is_set_ = false;
		Ns_is_set_ = false;
		Ni_is_set_ = false;
		Tr_is_set_ = false;
		illum_is_set_=false;
		name_="empty";
	}

	bool is_valid(void) const {
		return tex_is_set || Kd_is_set_ || Ka_is_set_ || Ks_is_set_ || Tr_is_set_;
	}

	bool has_Kd(void) const { return Kd_is_set_; }
	bool has_Ka(void) const { return Ka_is_set_; }
	bool has_Ks(void) const { return Ks_is_set_; }
	bool has_Ns(void) const { return Ns_is_set_; }
	bool has_Ni(void) const { return Ni_is_set_; }
	bool has_illum(void) const { return illum_is_set_; }
	bool has_Tr(void) const { return Tr_is_set_; }
	bool has_tex(void) const { return tex_is_set; }

	void set_Kd( float r, float g, float b ) {
		Kd_=Vec3Df(r,g,b); Kd_is_set_=true;
	}

	void set_Ka( float r, float g, float b ) {
		Ka_=Vec3Df(r,g,b); Ka_is_set_=true;
	}

	void set_Ks( float r, float g, float b ) {
		Ks_=Vec3Df(r,g,b); Ks_is_set_=true;
	}

	void set_Ns( float r) {
		Ns_=r; Ns_is_set_=true;
	}

	void set_Ni( float r) {
		Ni_=r; Ni_is_set_=true;
	}

	void set_illum( int r) {
		illum_=r; illum_is_set_=true;
	}

	void set_Tr( float t ) {
		Tr_=t; Tr_is_set_=true;
	}

	void set_textureName(const std::string& s)//name of the texture image file
	{
		textureName_=s; tex_is_set = true;
	}

	void set_name(const std::string & s )
	{ 
		name_=s;
	}

	const Vec3Df& Kd( void ) const { return Kd_; } //diffuse
	const Vec3Df& Ka( void ) const { return Ka_; } //ambiant
	const Vec3Df& Ks( void ) const { return Ks_; } //specular
	float  Ni( void ) const { return Ni_; } 
	float  Ns( void ) const { return Ns_; } //shininess
	int	   illum(void)const { return illum_;}
	float  Tr( void ) const { return Tr_; }//can be hijacked, e.g., for transparency
	const std::string & textureName()//name of the texture image file
	{
		return textureName_;
	}

	const std::string & name()//name of the material
	{
		return name_;
	}

	private:

	Vec3Df Kd_;	 bool Kd_is_set_;	// diffuse
	Vec3Df Ka_;	 bool Ka_is_set_;	// ambient
	Vec3Df Ks_;	 bool Ks_is_set_;	// specular
	float Ns_;	 bool Ns_is_set_;	// specular exponent
	float Ni_;	 bool Ni_is_set_;	// refraction index
	int illum_;	 bool illum_is_set_;	// illumination model
	float Tr_;	 bool Tr_is_set_;	// transperency
	std::string name_;
	std::string textureName_; bool tex_is_set;
};


/************************************************************
 * Triangle Class
 ************************************************************/
class Triangle {
	public:
	inline Triangle () {
		v[0] = v[1] = v[2] = 0;
	}
	inline Triangle (const Triangle & t2) {
		v[0] = t2.v[0];
		v[1] = t2.v[1];
		v[2] = t2.v[2];

		t[0] = t2.t[0];
		t[1] = t2.t[1];
		t[2] = t2.t[2];

	}
	inline Triangle (unsigned int v0, unsigned int t0, unsigned int v1, unsigned int t1, unsigned int v2, unsigned int t2) {
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;

		t[0] = t0;
		t[1] = t1;
		t[2] = t2;
	}
	inline virtual ~Triangle () {}
	inline Triangle & operator= (const Triangle & t2) {
		v[0] = t2.v[0];
		v[1] = t2.v[1];
		v[2] = t2.v[2];
		t[0] = t2.v[0];
		t[1] = t2.v[1];
		t[2] = t2.v[2];
		return (*this);
	}
	unsigned int v[3];
	unsigned int t[3];
};

/************************************************************
 * Class de maillage basique
 ************************************************************/
class Mesh {
public:
	Mesh(){}
	inline Mesh (const std::vector<Vertex> & v, const std::vector<Triangle> & t) : vertices (v), triangles (t)  {}
	bool loadMesh(const char * filename, bool randomizeTriangulation);
	bool loadMtl(const char * filename, std::map<std::string, unsigned int> & materialIndex);
	void computeVertexNormals ();
	void draw();
	void drawSmooth();

	//this is relevant for you:
	//Vertices are the vertex positions, textures, and normals of the mesh.
	std::vector<Vertex> vertices;
	//this is relevant for you:
	//texCoords are the texture coordinates, these are DIFFERENT indices in triangles.
	//in the current version, if you use textures, then you have to use texture coords everywhere...
	//I might send an update of the code to change this.
	//for convenience, Vec3Df is used, although only 2D tex coordinates are read corresponding to the x,y entry of Vec3Df.
	std::vector<Vec3Df> texcoords;
	//Triangles are the indices of the vertices involved in a triangle.
	//a triplet corresponds to one triangle. 
	//A Triangle contains the indeces of the three vertices that are neighboring
	std::vector<Triangle> triangles;
	//These are the material properties
	//each triangle (!) has a material. Use the triangle index to receive a material INDEX
	std::vector<unsigned int> triangleMaterials;
	//using the material index, you can then recover the material from this vector
	//the class material is defined just above
	std::vector<Material> materials;
};

#endif // MESH_H
