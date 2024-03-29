#include "image.h"
#include <stdio.h>

bool Image::writeImage(const char * filename)
{
	FILE* file;
    file = fopen(filename, "wb");
	if (!file)
	{
		printf("dump file problem... file\n");
		return false;
	}

	fprintf(file, "P6\n%i %i\n255\n",_width, _height);

	
	std::vector<unsigned char> imageC(_image.size());
	
	for (unsigned int i=0; i<_image.size();++i)
		imageC[i]=(unsigned char)(_image[i]*255.0f);
	
	int t = fwrite(&(imageC[0]), _width * _height * 3, 1, file);
	if (t!=1)
	{
		printf("Dump file problem... fwrite\n");
		return false;
	}

	fclose(file);
	return true;
}

bool Image::readImage(const char * filename)
{
	FILE* file;
	file = fopen(filename, "rb");
	if (!file) {
		printf("ERROR: No file called %s!\n", filename);
		return false;
	}	
	char* type[2];
	int width, height;
	fscanf(file, "%*s\n");
	char buf[256];
	while (fgets(buf, 256, file) && buf[0] == '#') {
		printf(buf);
	}

	sscanf(buf, "%i %i\n", &width, &height);
	fscanf(file, "255\n");
	this->_width = width;
	this->_height = height;
	std::vector<unsigned char> imageC(width * height * 3);
	int t = fread(&(imageC[0]), width * height * 3, 1, file);
	for (int i = 0; i < imageC.size(); i++) {
		_image.push_back((float)imageC[i]/255.0f);
	}
	printf("Loaded texture %s\n", filename);
	return true;
}

Vec3Df Texture::getColor(float tex_u, float tex_v) {
	Vec3Df rgb(0.f,0.f,0.f);
	int u = _image_data._width * tex_u - 1;
	int v = _image_data._height * tex_v - 1;
	for (int i = 0; i < 3; i++) {
		rgb[i] = _image_data._image[u * 3 + v * 3 * _image_data._width + i];
	}
	return rgb;
}

void Texture::convertBarycentricToTexCoord(float a, float b, Vec3Df* texcoords, float& tex_u, float& tex_v) {
	//calculate third barycentric coordinate
	float c = 1 - a - b;
	Vec3Df uv = c * texcoords[0] + a * texcoords[1] + b * texcoords[2];
	tex_u = uv[0];
	tex_v = uv[1];
}
