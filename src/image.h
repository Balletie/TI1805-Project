#ifndef IMAGE_H_
#define IMAGE_H_

class Image
{
	public:
	Image(int width, int height)
	: _width(width)
	, _height(height)
	{
		_image.resize(3*_width*_height);
	}
	void setPixel(int i, int j, const RGBValue & rgb)
	{
		_image[3*(_width*j+i)]=rgb[0];
		_image[3*(_width*j+i)+1]=rgb[1];
		_image[3*(_width*j+i)+2]=rgb[2];

	}
	std::vector<float> _image;
	int _width;
	int _height;

	bool writeImage(const char * filename);
};

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

#endif /* IMAGE_H_ */
