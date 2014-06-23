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
