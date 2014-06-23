#include "image.h"

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
	file = fopen(filename, "r");
	if (!file) {
		printf("ERROR: No such file\n");
		return false;
	}	
	char* type[2];
	int width, height;
	fscanf(file, "%s\n%i %i\n255\n", &type, &width, &height);
	printf("width %i height %i\n", width, height);
	this->_width = width;
	this->_height = height;
	std::vector<unsigned char> imageC(width * height * 3);
	int t = fread(&(imageC[0]), width * height * 3, 1, file);
	for (int i = 0; i < imageC.size(); i++) {
		_image.push_back((float)imageC[i]/255.0f);
		printf("%f",_image[i]);
	}
}

Vec3Df Texture::getColor(int u, int v) {
	Vec3Df rgb(0.f,0.f,0.f);
	for (int i = 0; i < 3; i++) {
		rgb[i] = _image_data[u * 3 + v * 3 * _width + i];
	}
	return rgb;
}

void Texture::convertBarycentricToTexCoord(float a, float b, Vec3Df* texcoords, int& tex_u, int& tex_v) {
	//calculate third barycentric coordinate
	float c = 1 - a - b;
	Vec3Df& uv = a * texcoords[0] + b * texcoords[1] + c * texcoords[2];
	tex_u = uv[0];
	tex_v = uv[1];
}
